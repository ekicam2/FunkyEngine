#include "FunkyEngine.h"

#include "DebugMacros.h"

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

#include "BasicTypes.h"

#include "Math/Camera.h"
#include "Rendering/ParticleSystem.h"

#include "Rendering/DX11/DX11ImGUIFasade.h" 
#include "Rendering/OpenGL/OGL_ImGUIFasade.h"
#include "Rendering/Renderer.h"

#include "Core/Containers.h"
#include "Core/String.h"

#include "Utils/MeshUtils.h"

#include "SDL.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return Funky::FunkyEngine::GetEngine()->ProcessInput(hWnd, Message, wParam, lParam);
}

namespace Funky
{
	FunkyEngine* FunkyEngine::_Engine = nullptr;
	Core::IO::IIOSystem* FunkyEngine::_IO = nullptr;

	FunkyEngine::FunkyEngine()
		: RenderingBackend(Rendering::RenderingBackend::API::DX11)
		, ThreadPool({ {Core::Thread::Type::Rendering, (u16)1u}, {Core::Thread::Type::Worker, (u16)5u} })
		, TaskManager(&ThreadPool)
	{
		FunkyEngine::_Engine = this;

		#ifdef FUNKY_EDITOR
			EditorWindowManager = new Editor::WindowManager();
		#endif // FUNKY_EDITOR
			AssetManager = new Funky::AssetRegistry();

		IOSystem = new Core::IO::WindowsIOSystem();
		FunkyEngine::_IO = IOSystem;
	}

	FunkyEngine::~FunkyEngine()
	{
		delete IOSystem;
		#ifdef FUNKY_EDITOR
			delete Editor;
			delete EditorWindowManager;
		#endif // FUNKY_EDITOR
	}

	Core::IO::IIOSystem* FunkyEngine::GetIO()
	{
		return _IO;
	}

	FunkyEngine* FunkyEngine::GetEngine()
	{
		return _Engine;
	}

#ifdef FUNKY_EDITOR
	Funky::Editor::WindowManager* FunkyEngine::GetEditorWindowManager()
	{
		return FunkyEngine::GetEngine()->EditorWindowManager;
	}
#endif // FUNKY_EDITOR

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

		Renderer = new Rendering::Renderer(RenderingBackend);
		Renderer->InitBuffers();

		#ifdef FUNKY_EDITOR
			Editor = new Editor::EditorContext();
		#endif // FUNKY_EDITOR

		return true;
	}

	void FunkyEngine::Run()
	{
		//RECT ClientArea;
		//GetClientRect(hWnd, &ClientArea);
		//f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
		//f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);

		std::unique_ptr<Asset::RawMesh> CubeMesh = std::make_unique<Asset::RawMesh>(MeshUtils::CreateCube());
		std::unique_ptr <Asset::RawMesh> SkySphere = std::make_unique<Asset::RawMesh>(MeshUtils::CreateSphere(2000.0f, true));

		str Textures[6] = {
 			"RealData/Textures/mp_troubled/troubled-waters_ft.tga",
			"RealData/Textures/mp_troubled/troubled-waters_bk.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_up.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_dn.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_rt.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_lf.tga"
 		};
		Asset::CubemapTexture* SkyTexture = Asset::CubemapTexture::CreateFromFile(Textures);
		CHECK(SkyTexture != nullptr);
		SkyTexture->Proxy = RenderingBackend.CreateCubemap(SkyTexture->GetData(), SkyTexture->Size);
		CHECK(SkyTexture != nullptr);

		
		/*Asset::Material SkyMaterial("RealData/Shaders/Sky.fkmat");
		Asset::Material DepthMaterial("RealData/Shaders/Depth.fkmat");
		Asset::Material LitMaterial("RealData/Shaders/Sample.fkmat");*/

		Asset::Material* SkyMaterial	= AssetManager->GetByName<Asset::Material>("Sky");
		[[maybe_unused]] Asset::Material* DepthMaterial	= AssetManager->GetByName<Asset::Material>("Depth");
		Asset::Material* LitMaterial	= AssetManager->GetByName<Asset::Material>("Sample");


		// Math::Camera ShadowCamera(DeltaX / DeltaY, 90.0f, 1.0f, 26.5f);
		// ShadowCamera.MakeOrtho(DeltaX / DeltaY, 1.0f, 1.0, 15.0f);
		// ShadowCamera.Translate({ 10.0f, 0.0f, 0.0f });
		// ShadowCamera.Rotate({ 0.0f, -90.0f, 0.0f });

		//ShadowCB.Projection = ShadowCamera.GetProjection();
		//ShadowCB.View = ShadowCamera.GetView();
		//RenderingBackend.UpdateConstantBuffer(ShadowCBHandle, (Rendering::RenderingBackend::ConstantBufferData)(&ShadowCB));


		for (u8 i = 0; i < 4; ++i)
		{
			auto NewDrawable = new Scene::Drawable();
			NewDrawable->Mesh.Mat = LitMaterial;
			//NewDrawable->Mesh.Targets.push_back(ShadowsRT);
			NewDrawable->Mesh.Data = CubeMesh.get();
			NewDrawable->Name = str("drawableObj_").append(std::to_string(i));
			NewDrawable->Position = Math::Vector3f(0.0f, 0.0f, 1.0f) * i;
			MainScene.SceneNodes.push_back(NewDrawable);
		}

		MainScene.SkySphere = new Scene::MeshComponent();
		MainScene.SkySphere->Data = SkySphere.get();
		MainScene.SkySphere->Mat = SkyMaterial;
		MainScene.SkySphere->Textures.push_back((ITexture*)SkyTexture);


		bool bGotMsg;

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

				IOSystem->Update();
			}
			else
			{
				#ifdef FUNKY_EDITOR
					Editor->Update();
					Renderer->DrawSceneFromView(&Editor->MainCamera, &MainScene);
				#endif // FUNKY_EDITOR

				DrawGUI();
				RenderingBackend.Present();
			}
		}

		delete MainScene.SkySphere;
	}

	bool FunkyEngine::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		return true;
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

		ImGui::ShowDemoWindow();

		#ifdef FUNKY_EDITOR
			EditorWindowManager->DrawGUI();
		#endif // FUNKY_EDITOR

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