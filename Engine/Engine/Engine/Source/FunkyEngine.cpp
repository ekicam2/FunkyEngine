#include "FunkyEngine.h"

#include "DebugMacros.h"

#include <fstream>

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

#include "Rendering/Camera.h"
#include "Rendering/Texture.h"
#include "Rendering/ParticleSystem.h"

#include "DX11/DX11ImGUIFasade.h" 

#include "BasicTypes.h"
#include "Core/Containers.h"
#include "Core/String.h"

#include "SDL.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Funky::FunkyEngine::GetEngine().ProcessInput(hWnd, message, wParam, lParam);
}

namespace Funky
{
	FunkyEngine* FunkyEngine::Engine = nullptr;
	IOSystem* FunkyEngine::IO = nullptr;

	FunkyEngine::FunkyEngine()
		: RenderingBackend(Rendering::RenderingBackend::API::DX11)
	{
		FunkyEngine::Engine = this;
		FunkyEngine::IO = new IOSystem();
	}

	IOSystem const & FunkyEngine::GetIO()
	{
		return *IO;
	}

	FunkyEngine & FunkyEngine::GetEngine()
	{
		return *Engine;
	}

	LRESULT __stdcall FunkyEngine::ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
			return true;
		// else fill IO struct
			
		// sort through and find what code to run for the message given
		switch (message)
		{
		case WM_SIZE:
			{
				static i32 counter = 0;
				if (counter > 0)
				{
					const Math::Vector2u NewSize{ LOWORD(lParam), HIWORD(lParam) };
					RenderingBackend.OnViewportResized(NewSize);
				}
				++counter;
			}
			break;
			// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} break;
		}

		// Handle any messages the switch statement didn't
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	bool FunkyEngine::Init()
	{
		constexpr unsigned uWindowWidth = 2048u;
		constexpr unsigned uWindowHeight = 1024u;

		if (!CreateAndShowWindow({ uWindowWidth, uWindowHeight }))
			return false;

		if (SDL_Init(SDL_INIT_AUDIO) != 0)
		{
			LOG_ERROR_FUNKY(TEXT("SDL_Init: Failed to init SDL!\n"));
			return false;
		}
		atexit(SDL_Quit);

		int flags = IMG_INIT_JPG | IMG_INIT_PNG;
		int initted = IMG_Init(flags);
		if ((initted&flags) != flags)
		{
			LOG_ERROR_FUNKY(TEXT("IMG_Init: Failed to init required jpg and png support!\n"));
			LOG_ERROR_FUNKY(TEXT("IMG_Init: "), IMG_GetError());
			return false;
		}
		atexit(IMG_Quit);

		
		//LOG_FUNKY(fk::getExecPath());

		// IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hwnd);
		// IMGUI END

		if (!RenderingBackend.Init(hwnd))
			return false;

		return true;
	}

	void FunkyEngine::Run()
	{
		DEFINE_CONSTANT_BUFFER_BEGIN(BaseConstantBuffer)
			BaseConstantBuffer() : LookAt(0.0f, 0.0f, 0.0f) {}
			DirectX::XMMATRIX MVP;
			DirectX::XMMATRIX Model;
			Funky::Math::Vector3f LookAt;
		DEFINE_CONSTANT_BUFFER_END(BaseConstantBuffer)

		BaseConstantBuffer MVPBuffer;
		Rendering::RenderingBackend::ConstantBuffer MVPBufferHandle = RenderingBackend.CreateConstantBuffer(sizeof(BaseConstantBuffer));

		DEFINE_CONSTANT_BUFFER_BEGIN(ShadowConstantBuffer)
			ShadowConstantBuffer() {}
			DirectX::XMMATRIX View;
			DirectX::XMMATRIX Projection;
		DEFINE_CONSTANT_BUFFER_END(ShadowConstantBuffer)

		ShadowConstantBuffer ShadowCB;
		Rendering::RenderingBackend::ConstantBuffer ShadowCBHandle = RenderingBackend.CreateConstantBuffer(sizeof(ShadowConstantBuffer));

		RawMesh* CubeMesh = MeshUtils::CreateCube();

		char const* textures[] = {
 			"Resource/Textures/mp_troubled/troubled-waters_rt.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_lf.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_up.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_dn.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_bk.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_ft.tga"
 		};
		CubemapTexture* SkyTexture = CubemapTexture::CreateFromFile(textures);
		SkyTexture->Proxy = RenderingBackend.CreateCubemap(SkyTexture->GetData(), SkyTexture->Size);

		RawMesh* SkySphere = MeshUtils::CreateSphere(2000.0f, true);
		SkySphere->Proxy = RenderingBackend.CreateMeshProxy(SkySphere);

		//DirectUtils::InputLayout::PositionColorUV;
		Rendering::RenderingBackend::VertexShader skyvs = [&]() {
			std::ifstream input("Shaders\\SkyVertexShader.cso", std::ios::binary);
			darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}();

		Rendering::RenderingBackend::PixelShader skyps = [&]() {
			std::ifstream input("Shaders\\SkyPixelShader.cso", std::ios::binary);
			darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}();

		Rendering::RenderingBackend::VertexShader depthvs = [&]() {
			std::ifstream input("Shaders\\DepthVertexShader.cso", std::ios::binary);
			darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}();

		Rendering::RenderingBackend::PixelShader depthps = [&]() {
			std::ifstream input("Shaders\\DepthPixelShader.cso", std::ios::binary);
			darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}();

		RECT ClientArea;
		GetClientRect(hwnd, &ClientArea);

		f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
		f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);

		Camera SceneCamera(DeltaX / DeltaY, 90.0f, 0.01f, 3000.0f);
		//SceneCamera.MakeOrtho((DeltaX / DeltaY) * 1.0f, 1.0f, 0.01f, 10000.0f);


		//TODO(ekicam2): need to create depth buffer for render target of the same size!!!!!!
		//TODO(ekicam2): and when binding render target I need to bind associated DB as well
		Rendering::RenderingBackend::RenderTarget ProjectionRenderTarget = RenderingBackend.CreateRenderTarget({ (u32)DeltaX, (u32)DeltaY });
		Camera ProjectionCamera(DeltaX / DeltaY, 90.0f, 1.0f, 26.5f);
		//ProjectionCamera.MakeOrtho(DeltaX / DeltaY, 1.0f, 1.0, 15.0f);
		ProjectionCamera.Translate({ 10.0f, 0.0f, 0.0f });
		ProjectionCamera.Rotate({ 0.0f, -90.0f, 0.0f });

		ShadowCB.Projection = ProjectionCamera.GetProjection();
		ShadowCB.View = ProjectionCamera.GetView();

		RenderingBackend.UpdateConstantBuffer(ShadowCBHandle, (Rendering::RenderingBackend::ConstantBufferData)(&ShadowCB));

		Rendering::RenderingBackend::VertexShader vs = [&]() {
			std::ifstream input("Shaders\\SampleVertexShader.cso", std::ios::binary);
			darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}(); 

		Rendering::RenderingBackend::PixelShader ps = [&]() {
			std::ifstream input("Shaders\\SamplePixelShader.cso", std::ios::binary);
			darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
			input.close();
			return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
		}();

		for (u8 i = 0; i < 4; ++i)
		{
			auto NewDrawable = new Scene::Drawable();
			NewDrawable->Mesh = CubeMesh;
			NewDrawable->Name = str("drawableObj_").append(std::to_string(i));
			NewDrawable->Position = Math::Vector3f(0.0f, 0.0f, 1.0f) * i;
			MainScene.SceneNodes.push_back(NewDrawable);
		}

		ParticleSystem PS;
		PS.Emiters.push_back(ParticleSystem::ParticleEmitter());

		for (u32 i = 0; i < 10; ++i)
		{
			ParticleSystem::ParticleEmitter::Particle Particle;
			Particle.Lifetime = 1.0f;
			PS.Emiters[0].Particles.push_back(std::move(Particle));
		}

		bool bGotMsg;
		bool bPrevFrameRMB;

		MSG  msg;
		msg.message = WM_NULL;
		while (WM_QUIT != msg.message)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
			if (bGotMsg)
			{
				// Translate and dispatch the message
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_SIZE)
				{
					[[maybe_unused]] i32 i = 0;
				}

				[=](HWND hwnd, bool& bPrevFrameRMB, Camera& SceneCamera)
				{
					constexpr unsigned char PressedMask = 1 << 7;
					 
					if (!ImGui::GetIO().WantCaptureKeyboard)
					{
						if (PressedMask & GetKeyState('W'))
					 		SceneCamera.Translate(Math::Vector3f(0.0f, 0.0f, 1.f) * CameraSpeed);
						if (PressedMask & GetKeyState('S'))
					 		SceneCamera.Translate(Math::Vector3f(0.0f, 0.0f, -1.0f) *CameraSpeed);
						if (PressedMask & GetKeyState('A'))
					 		SceneCamera.Translate(Math::Vector3f(-1.0f, 0.0f, 0.0f) * CameraSpeed);
						if (PressedMask & GetKeyState('D'))
					 		SceneCamera.Translate(Math::Vector3f(1.0f, 0.0f, 0.0f) * CameraSpeed);
						if (PressedMask & GetKeyState('E'))
							SceneCamera.Translate(Math::Vector3f(0.0f, 1.0f, 0.0f) * CameraSpeed);
						if (PressedMask & GetKeyState('Q'))
							SceneCamera.Translate(Math::Vector3f(0.0f, -1.0f, 0.0f) * CameraSpeed);
					}

					if (!ImGui::GetIO().WantCaptureMouse && PressedMask & GetKeyState(VK_RBUTTON))
					{
					 	RECT WindowRect = { 0 };
					 	GetWindowRect(hwnd, &WindowRect); //get window rect of control relative to screen
					 	//GetClientRect(hwnd, &WindowRect);
					 	POINT CursorPosition = { 0, 0 };
					 	GetCursorPos(&CursorPosition);
					 	POINT WindowCenter = {
					 		WindowRect.left + ((WindowRect.right - WindowRect.left) / 2),
					 		WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2)
					 	};
					 
					 	POINT MouseOffset = {
					 		CursorPosition.x - WindowCenter.x,
					 		CursorPosition.y - WindowCenter.y
					 	};
					 
					 	if (bPrevFrameRMB)
					 	{
					 		//SetCursorPos(WindowCenter.x, WindowCenter.y);
					 		SceneCamera.RotateClamped(
								Funky::Math::Vector3f(MouseOffset.y * CameraY, MouseOffset.x * CameraX, 0.0f),
								Funky::Math::Vector3f(1.0f, 0.0f, 0.0f),
								Funky::Math::Vector3f(89.0f, 0.0f, 0.0f)
							);
					 	}
					 
					 	SetCursorPos(WindowCenter.x, WindowCenter.y);

						bPrevFrameRMB = true;
					}
					else
					{
					 	bPrevFrameRMB = false;
					}
				}(hwnd, bPrevFrameRMB, SceneCamera);
			}
			else
			{
				// DeltaTime is not used atm
				PS.Update(1.0f/60.0f);

				auto renderScene = [&MVPBuffer, &SceneCamera, &MVPBufferHandle, &CubeMesh, &vs, &ps, &skyvs, &skyps, SkySphere, SkyTexture, this, &PS, &ProjectionRenderTarget, &ProjectionCamera, &depthvs, &depthps, &ShadowCBHandle]() {

					//projection
					{
						//			PREPARE FRAME
						RenderingBackend.BindRenderTarget(ProjectionRenderTarget);
						RenderingBackend.ClearRenderTargetWithColor({ 0.0f, 0.0f, 0.0f }, ProjectionRenderTarget);
						RenderingBackend.ClearDepthStencil(1.0f, 0u);
						RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
						//			PREPARE FRAME END 

						MVPBuffer.LookAt = ProjectionCamera.GetLookat().Normalize();

						for (u64 i = 0; i < MainScene.SceneNodes.size(); ++i)
						{
							Scene::Drawable* Temp = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[i]);
							if (Temp && Temp->Mesh && Temp->bVisible)
							{
								if (!Temp->Mesh->HasValidProxy())
								{
									Temp->Mesh->Proxy = RenderingBackend.CreateMeshProxy(Temp->Mesh);
								}

								MVPBuffer.Model = DirectX::XMMatrixScaling(Temp->Scale.X, Temp->Scale.Y, Temp->Scale.Z)
									* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Temp->Rotation.X), Math::ToRad(Temp->Rotation.Y), Math::ToRad(Temp->Rotation.Z))
									* DirectX::XMMatrixTranslation(Temp->Position.X, Temp->Position.Y, Temp->Position.Z);
								MVPBuffer.MVP = MVPBuffer.Model * ProjectionCamera.GetView() * ProjectionCamera.GetProjection();

								// DRAW SCENE
								RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

								RenderingBackend.BindVertexShader(depthvs);
								RenderingBackend.BindPixelShader(depthps);
								RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
								RenderingBackend.DrawMesh(Temp->Mesh->Proxy);
								// DRAW SCENE END
							}
						}

						MVPBuffer.Model = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
						MVPBuffer.MVP = MVPBuffer.Model * ProjectionCamera.GetView() * ProjectionCamera.GetProjection();
						RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

						RenderingBackend.BindDefaultRenderTarget();
					}
					
					//			PREPARE FRAME
					RenderingBackend.BindDefaultRenderTarget();
					RenderingBackend.ClearRenderTargetWithColor({ 0.392156899f, 0.584313750f, 0.929411829f });
					RenderingBackend.ClearDepthStencil(1.0f, 0u);
					RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
					//			PREPARE FRAME END 

					MVPBuffer.LookAt = SceneCamera.GetLookat().Normalize();


					for (u32 i = 0; i < PS.Emiters[0].Particles.size(); ++i)
					{
						auto& Particle = PS.Emiters[0].Particles[i];

						MVPBuffer.Model = DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f)
							* DirectX::XMMatrixTranslation(Particle.Position.X + i, Particle.Position.Y, Particle.Position.Z);
						MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();

						RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

						RenderingBackend.BindVertexShader(vs);
						RenderingBackend.BindPixelShader(ps);
						RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);

						//TODO(ekicam2): remove this HACK
						Scene::Drawable* Temp = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[1]);
						if (!Temp->Mesh->HasValidProxy())
						{
							Temp->Mesh->Proxy = RenderingBackend.CreateMeshProxy(Temp->Mesh);
						}
						RenderingBackend.DrawMesh(Temp->Mesh->Proxy);
					}

					for (u64 i = 0; i < MainScene.SceneNodes.size(); ++i)
					{
						Scene::Drawable* Drawable = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[i]);
						if (Drawable && Drawable->Mesh && Drawable->bVisible)
						{
							if (!Drawable->Mesh->HasValidProxy())
							{
								Drawable->Mesh->Proxy = RenderingBackend.CreateMeshProxy(Drawable->Mesh);
							}

							MVPBuffer.Model = DirectX::XMMatrixScaling(Drawable->Scale.X, Drawable->Scale.Y, Drawable->Scale.Z)
								* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Drawable->Rotation.X), Math::ToRad(Drawable->Rotation.Y), Math::ToRad(Drawable->Rotation.Z))
								* DirectX::XMMatrixTranslation(Drawable->Position.X, Drawable->Position.Y, Drawable->Position.Z);
							MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();

							// DRAW SCENE
							RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));
							RenderingBackend.BindVertexShader(vs);
							RenderingBackend.BindPixelShader(ps);
							RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
							RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, ShadowCBHandle, 1u);
							RenderingBackend.BindTexture(Rendering::RenderingBackend::ShaderResourceStage::Fragment, ProjectionRenderTarget);
							RenderingBackend.DrawMesh(Drawable->Mesh->Proxy);
							// DRAW SCENE END
						}
						else
						{
							static Rendering::RenderingBackend::MeshProxy TerrainProxy = Rendering::RenderingBackend::INVALID_INDEX;

							Scene::Terrain* Terrain = dynamic_cast<Scene::Terrain*>(MainScene.SceneNodes[i]);
							if (Terrain && Terrain->bVisible)
							{
								for (auto& TerrainNode : Terrain->TerrainNodes)
								{
									if (TerrainProxy == Rendering::RenderingBackend::INVALID_INDEX)
									{
										TerrainProxy = RenderingBackend.CreateMeshProxy(TerrainNode);
									}

									MVPBuffer.Model = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
									MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();

									// DRAW SCENE
									RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

									RenderingBackend.BindVertexShader(vs);
									RenderingBackend.BindPixelShader(ps);
									RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
									RenderingBackend.DrawMesh(TerrainProxy);
									// DRAW SCENE END
								}
							}
						}
					}

					MVPBuffer.Model = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
					MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();
					RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

					RenderingBackend.BindVertexShader(skyvs);
					RenderingBackend.BindPixelShader(skyps);
					RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
					RenderingBackend.BindTexture(Rendering::RenderingBackend::ShaderResourceStage::Fragment, SkyTexture->Proxy);
					RenderingBackend.DrawMesh(SkySphere->Proxy);
				};

				renderScene();

				DrawGUI();

				RenderingBackend.Present();
			}
		}

		delete CubeMesh;
	}

	bool FunkyEngine::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		return true;
	}

	void FunkyEngine::RenderScene()
	{
		//			PREPARE FRAME
		RenderingBackend.BindDefaultRenderTarget();
		RenderingBackend.ClearRenderTargetWithColor({ 0.392156899f, 0.584313750f, 0.929411829f });
		RenderingBackend.ClearDepthStencil(1.0f, 0u);
		RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
		//			PREPARE FRAME END 
	}

	void FunkyEngine::DrawGUI()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiWindowFlags WindowFlags = 0;
		WindowFlags |= ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Editor", NULL, WindowFlags);
			ImGui::DragFloat("Camera speed", &CameraSpeed, 0.01f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Camera X speed", &CameraX, 0.01f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Camera Y speed", &CameraY, 0.01f, 0.0f, 10.10f, "%.2f");
		ImGui::End();

		ImGui::ShowDemoWindow();
		MainScene.DrawGUI();

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	bool FunkyEngine::CreateAndShowWindow(Math::Vector2u const & windowSize)
	{
		constexpr charx const * const WND_CLASS_NAME = TEXT("MainWindowClass");

		WNDCLASSW wcx;
		wcx.style = CS_HREDRAW | CS_VREDRAW;
		wcx.lpfnWndProc = ::ProcessInput;							// points to window procedure 
		wcx.cbClsExtra = 0;											// no extra class memory 
		wcx.cbWndExtra = 0;											// no extra window memory 
		wcx.hInstance = hInstance;									// handle to instance 
		wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// predefined app. icon 
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);					// predefined arrow 
		wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background brush 
		wcx.lpszMenuName = TEXT("MainMenu");								// name of menu resource 
		wcx.lpszClassName = WND_CLASS_NAME;							// name of window class 
		if (!RegisterClass(&wcx))
		{
			BREAK();
			LOG_ERROR_FUNKY(TEXT("couldn't register a class with name "), WND_CLASS_NAME);
			return false;
		}

		hwnd = CreateWindow(WND_CLASS_NAME, TEXT("Window name"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowSize.X, windowSize.Y, NULL, NULL, hInstance, NULL);
		if (!hwnd)
		{
			BREAK();
			LOG_ERROR_FUNKY(TEXT("couldn't create the window with class: "), WND_CLASS_NAME);
			UnregisterClass(WND_CLASS_NAME, hInstance);
			return false;
		}

		ShowWindow(hwnd, SW_SHOW);

		return true;
	}

}