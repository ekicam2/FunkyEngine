#include "FunkyEngine.h"

#include "DebugMacros.h"

#include <fstream>

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

#include "Math/Camera.h"
#include "Rendering/TextureBase.h"
#include "Rendering/ParticleSystem.h"

#include "Rendering/DX11/DX11ImGUIFasade.h" 
#include "Rendering/OpenGL/OGL_ImGUIFasade.h"

#include "BasicTypes.h"
#include "Core/Containers.h"
#include "Core/String.h"

#include "SDL.h"

DEFINE_CONSTANT_BUFFER_BEGIN(BaseConstantBuffer)
BaseConstantBuffer() : LookAt(0.0f, 0.0f, 0.0f) {}
DirectX::XMMATRIX MVP;
DirectX::XMMATRIX Model;
Funky::Math::Vector3f LookAt;
DEFINE_CONSTANT_BUFFER_END(BaseConstantBuffer)

DEFINE_CONSTANT_BUFFER_BEGIN(ShadowConstantBuffer)
ShadowConstantBuffer() {}
DirectX::XMMATRIX View;
DirectX::XMMATRIX Projection;
DEFINE_CONSTANT_BUFFER_END(ShadowConstantBuffer)

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return Funky::FunkyEngine::GetEngine().ProcessInput(hWnd, Message, wParam, lParam);
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

	LRESULT CALLBACK FunkyEngine::ProcessInput(HWND InhWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(InhWnd, Message, wParam, lParam))
			return true;
		// else fill IO struct
			
		// sort through and find what code to run for the message given
		switch (Message)
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
		return DefWindowProc(InhWnd, Message, wParam, lParam);
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

		ImGui_ImplWin32_Init(hWnd);
		// IMGUI END

		if (!RenderingBackend.Init(hWnd))
			return false;

		return true;
	}

	void FunkyEngine::Run()
	{
		RECT ClientArea;
		GetClientRect(hWnd, &ClientArea);
		f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
		f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);

		// Shadow target
		Rendering::RenderingBackend::RenderTarget ShadowsRT = RenderingBackend.CreateRenderTarget({ (u32)DeltaX, (u32)DeltaY });

		BaseConstantBuffer MVPBuffer;
		ShadowConstantBuffer ShadowCB;

		Rendering::RenderingBackend::ConstantBuffer MVPBufferHandle = RenderingBackend.CreateConstantBuffer(sizeof(BaseConstantBuffer));
		Rendering::RenderingBackend::ConstantBuffer ShadowCBHandle = RenderingBackend.CreateConstantBuffer(sizeof(ShadowConstantBuffer));


		RawMesh* CubeMesh = MeshUtils::CreateCube();
		RawMesh* SkySphere = MeshUtils::CreateSphere(2000.0f, true);
		SkySphere->Proxy = RenderingBackend.CreateMeshProxy(SkySphere);


		char const* Textures[] = {
 			"Resource/Textures/mp_troubled/troubled-waters_rt.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_lf.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_up.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_dn.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_bk.tga",
 			"Resource/Textures/mp_troubled/troubled-waters_ft.tga"
 		};
		CubemapTexture* SkyTexture = CubemapTexture::CreateFromFile(Textures);
		CHECK(SkyTexture != nullptr);
		SkyTexture->Proxy = RenderingBackend.CreateCubemap(SkyTexture->GetData(), SkyTexture->Size);
		CHECK(SkyTexture != nullptr);

		
		Material SkyMaterial{
			[&]() {
				std::ifstream Input("Shaders\\SkyVertexShader.cso", std::ios::binary);
				darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
				Input.close();
				return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			[&]() {
				std::ifstream Input("Shaders\\SkyPixelShader.cso", std::ios::binary);
				darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
				Input.close();
				return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			{MVPBufferHandle}
		};
		Material DepthMaterial{
			[&]() {
				std::ifstream input("Shaders\\DepthVertexShader.cso", std::ios::binary);
				darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
				input.close();
				return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			[&]() {
				std::ifstream input("Shaders\\DepthPixelShader.cso", std::ios::binary);
				darray<byte> ShaderBinaryBuffer(std::istreambuf_iterator<char>(input), {});
				input.close();
				return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			{ MVPBufferHandle, ShadowCBHandle }
		};
		Material LitMaterial{
			[&]() {
				std::ifstream Input("Shaders\\SampleVertexShader.cso", std::ios::binary);
				darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
				Input.close();
				return RenderingBackend.CreateVertexShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			[&]() {
				std::ifstream Input("Shaders\\SamplePixelShader.cso", std::ios::binary);
				darray<BYTE> ShaderBinaryBuffer(std::istreambuf_iterator<char>(Input), {});
				Input.close();
				return RenderingBackend.CreatePixelShader(ShaderBinaryBuffer.data(), ShaderBinaryBuffer.size());
			}(),
			{ MVPBufferHandle }
		};


		//Camera setup
		Math::Camera SceneCamera(DeltaX / DeltaY, 90.0f, 0.01f, 3000.0f);
		//SceneCamera.MakeOrtho((DeltaX / DeltaY) * 1.0f, 1.0f, 0.01f, 10000.0f);

		Math::Camera ShadowCamera(DeltaX / DeltaY, 90.0f, 1.0f, 26.5f);
		//ProjectionCamera.MakeOrtho(DeltaX / DeltaY, 1.0f, 1.0, 15.0f);
		ShadowCamera.Translate({ 10.0f, 0.0f, 0.0f });
		ShadowCamera.Rotate({ 0.0f, -90.0f, 0.0f });
		ShadowCB.Projection = ShadowCamera.GetProjection();
		ShadowCB.View = ShadowCamera.GetView();
		RenderingBackend.UpdateConstantBuffer(ShadowCBHandle, (Rendering::RenderingBackend::ConstantBufferData)(&ShadowCB));


		for (u8 i = 0; i < 4; ++i)
		{
			auto NewDrawable = new Scene::Drawable();
			NewDrawable->Mesh.Mat = LitMaterial;
			NewDrawable->Mesh.Targets.push_back(ShadowsRT);
			NewDrawable->Mesh.Data = CubeMesh;
			NewDrawable->Name = str("drawableObj_").append(std::to_string(i));
			NewDrawable->Position = Math::Vector3f(0.0f, 0.0f, 1.0f) * i;
			MainScene.SceneNodes.push_back(NewDrawable);
		}

		bool bGotMsg;
		bool bPrevFrameRMB;

		MSG  Msg;
		Msg.message = WM_NULL;
		while (WM_QUIT != Msg.message)
		{
			bGotMsg = (PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE) != 0);
			if (bGotMsg)
			{
				// Translate and dispatch the message
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);

				if (Msg.message == WM_SIZE)
				{
					[[maybe_unused]] i32 i = 0;
				}

				[=](HWND hWnd, bool& bPrevFrameRMB, Math::Camera& SceneCamera)
				{
					constexpr byte PressedMask = 1 << 7;
					 
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
					 	GetWindowRect(hWnd, &WindowRect); //get window rect of control relative to screen
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
				}(hWnd, bPrevFrameRMB, SceneCamera);
			}
			else
			{
				//TODO(ekicam2): separate this lambda to more classes such as: Renderer, AssetManager and Scene
				auto renderScene = [&MVPBuffer, &SceneCamera, &MVPBufferHandle, &CubeMesh, &DepthMaterial, &SkyMaterial, SkySphere, SkyTexture, this, &ShadowsRT, &ShadowCamera, &ShadowCBHandle]() {

					//projection
					{
						//			PREPARE FRAME
						RenderingBackend.BindRenderTarget(ShadowsRT);
						RenderingBackend.ClearRenderTargetWithColor({ 0.0f, 0.0f, 0.0f }, ShadowsRT);
						RenderingBackend.ClearDepthStencil(1.0f, 0u);
						RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
						//			PREPARE FRAME END 

						MVPBuffer.LookAt = ShadowCamera.GetLookat().Normalize();

						for (u64 i = 0; i < MainScene.SceneNodes.size(); ++i)
						{
							Scene::Drawable* Temp = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[i]);
							if (Temp && Temp->bVisible)
							{
								if (!Temp->Mesh.Data->HasValidProxy())
								{
									Temp->Mesh.Data->Proxy = RenderingBackend.CreateMeshProxy(Temp->Mesh.Data);
								}

								MVPBuffer.Model = DirectX::XMMatrixScaling(Temp->Scale.X, Temp->Scale.Y, Temp->Scale.Z)
									* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Temp->Rotation.X), Math::ToRad(Temp->Rotation.Y), Math::ToRad(Temp->Rotation.Z))
									* DirectX::XMMatrixTranslation(Temp->Position.X, Temp->Position.Y, Temp->Position.Z);
								MVPBuffer.MVP = MVPBuffer.Model * ShadowCamera.GetView() * ShadowCamera.GetProjection();

								// DRAW SCENE
								RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

								
								RenderingBackend.BindVertexShader(DepthMaterial.VS);
								RenderingBackend.BindPixelShader(DepthMaterial.PS);
								RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
								RenderingBackend.DrawMesh(Temp->Mesh.Data->Proxy);
								// DRAW SCENE END
							}
						}

						MVPBuffer.Model = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
						MVPBuffer.MVP = MVPBuffer.Model * ShadowCamera.GetView() * ShadowCamera.GetProjection();
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

					for (u64 i = 0; i < MainScene.SceneNodes.size(); ++i)
					{
						Scene::Drawable* Drawable = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[i]);
						if (Drawable && Drawable->bVisible)
						{
							if (!Drawable->Mesh.Data->HasValidProxy())
							{
								Drawable->Mesh.Data->Proxy = RenderingBackend.CreateMeshProxy(Drawable->Mesh.Data);
							}

							MVPBuffer.Model = DirectX::XMMatrixScaling(Drawable->Scale.X, Drawable->Scale.Y, Drawable->Scale.Z)
								* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Drawable->Rotation.X), Math::ToRad(Drawable->Rotation.Y), Math::ToRad(Drawable->Rotation.Z))
								* DirectX::XMMatrixTranslation(Drawable->Position.X, Drawable->Position.Y, Drawable->Position.Z);
							MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();

							// DRAW SCENE
							RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));
							RenderingBackend.BindVertexShader(Drawable->Mesh.Mat.VS);
							RenderingBackend.BindPixelShader(Drawable->Mesh.Mat.PS);
							RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
							RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, ShadowCBHandle, 1u);
							RenderingBackend.BindTexture(Rendering::RenderingBackend::ShaderResourceStage::Fragment, ShadowsRT);
							RenderingBackend.DrawMesh(Drawable->Mesh.Data->Proxy);
							// DRAW SCENE END
						}
						//else
						//{
						//	static Rendering::RenderingBackend::MeshProxy TerrainProxy = Rendering::RenderingBackend::INVALID_INDEX;
						//
						//	Scene::Terrain* Terrain = dynamic_cast<Scene::Terrain*>(MainScene.SceneNodes[i]);
						//	if (Terrain && Terrain->bVisible)
						//	{
						//		for (auto& TerrainNode : Terrain->TerrainNodes)
						//		{
						//			if (TerrainProxy == Rendering::RenderingBackend::INVALID_INDEX)
						//			{
						//				TerrainProxy = RenderingBackend.CreateMeshProxy(TerrainNode);
						//			}
						//
						//			MVPBuffer.Model = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
						//			MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();
						//
						//			// DRAW SCENE
						//			RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));
						//
						//			RenderingBackend.BindVertexShader(Vs);
						//			RenderingBackend.BindPixelShader(Ps);
						//			RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
						//			RenderingBackend.DrawMesh(TerrainProxy);
						//			// DRAW SCENE END
						//		}
						//	}
						//}
					}

					MVPBuffer.Model = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
					MVPBuffer.MVP = MVPBuffer.Model * SceneCamera.GetView() * SceneCamera.GetProjection();
					RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

					RenderingBackend.BindVertexShader(SkyMaterial.VS);
					RenderingBackend.BindPixelShader(SkyMaterial.PS);
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
		
	}

	void FunkyEngine::DrawGUI()
	{
		switch (RenderingBackend.GetBackendAPI())
		{
		case Rendering::RenderingBackend::API::DX11:
			ImGui_ImplDX11_NewFrame();
			break;
		case Rendering::RenderingBackend::API::OGL:
			ImGui_ImplOpenGL3_NewFrame();
			break;
		}

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

		switch (RenderingBackend.GetBackendAPI())
		{
		case Rendering::RenderingBackend::API::DX11:
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			break;
		case Rendering::RenderingBackend::API::OGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			break;
		}
	}

	bool FunkyEngine::CreateAndShowWindow(Math::Vector2u const & windowSize)
	{
		constexpr charx const * const WND_CLASS_NAME = TEXT("MainWindowClass");

		WNDCLASSW Wcx;
		Wcx.style			= CS_HREDRAW | CS_VREDRAW;
		Wcx.lpfnWndProc		= ::ProcessInput;						// points to window procedure 
		Wcx.cbClsExtra		= 0;							  		// no extra class memory 
		Wcx.cbWndExtra		= 0;							  		// no extra window memory 
		Wcx.hInstance		= hInstance;					  		// handle to instance 
		Wcx.hIcon			= LoadIcon(NULL, IDI_APPLICATION);		// predefined app. icon 
		Wcx.hCursor			= LoadCursor(NULL, IDC_ARROW);	  		// predefined arrow 
		Wcx.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background brush 
		Wcx.lpszMenuName	= TEXT("MainMenu");						// name of menu resource 
		Wcx.lpszClassName	= WND_CLASS_NAME;						// name of window class 
		if (!RegisterClass(&Wcx))
		{
			BREAK();
			LOG_ERROR_FUNKY(TEXT("couldn't register a class with name "), WND_CLASS_NAME);
			return false;
		}

		hWnd = CreateWindow(WND_CLASS_NAME, TEXT("Window name"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowSize.X, windowSize.Y, NULL, NULL, hInstance, NULL);
		if (!hWnd)
		{
			BREAK();
			LOG_ERROR_FUNKY(TEXT("couldn't create the window with class: "), WND_CLASS_NAME);
			UnregisterClass(WND_CLASS_NAME, hInstance);
			return false;
		}

		ShowWindow(hWnd, SW_SHOW);

		return true;
	}

}