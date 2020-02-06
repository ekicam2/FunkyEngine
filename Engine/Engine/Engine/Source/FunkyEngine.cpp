#include "FunkyEngine.h"

#include "DebugMacros.h"

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

#include "BasicTypes.h"

#include "Math/Vector2.h"
#include "Math/Camera.h"
#include "Rendering/ParticleSystem.h"

#include "Rendering/DX11/DX11ImGUIFasade.h" 
#include "Rendering/OpenGL/OGL_ImGUIFasade.h"
#include "Rendering/Renderer.h"

#include "Core/Containers.h"
#include "Core/String.h"


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
		, IOSystem(new Core::IO::WindowsIOSystem())
	{
		FunkyEngine::_Engine = this;
		FunkyEngine::_IO = IOSystem;

		#ifdef FUNKY_EDITOR
			EditorWindowManager = new Editor::WindowManager();
		#endif // FUNKY_EDITOR
		
		AssetManager = new Funky::AssetRegistry();
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
		CHECK(_IO);
		return _IO;
	}

	FunkyEngine* FunkyEngine::GetEngine()
	{
		CHECK(_Engine);
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
		case WM_EXITSIZEMOVE:
			{
				RenderingBackend.OnViewportResized(Math::Vec2u(0u, 0u));
				
#ifdef FUNKY_EDITOR
				RECT ClientArea;
				GetClientRect(FunkyEngine::GetEngine()->hWnd, &ClientArea);
				f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
				f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);
				
				Editor->MainCamera.MakePerepective(DeltaX / DeltaY, 90.0f, 0.01f, 3000.0f);
#endif
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

		LOG("Creating widnow with size: ", uWindowWidth, "x", uWindowHeight);

		if (!CreateAndShowWindow({ uWindowWidth, uWindowHeight }))
			return false;

		LOG("Init SDL 2.0");
		if (SDL_Init(SDL_INIT_AUDIO) != 0)
		{
			LOG_ERROR(TEXT("SDL_Init: Failed to init SDL!\n"));
			return false;
		}
		atexit(SDL_Quit);

		LOG("Init SDL IMAGE");
		int flags = IMG_INIT_JPG | IMG_INIT_PNG;
		int initted = IMG_Init(flags);
		if ((initted&flags) != flags)
		{
			LOG_ERROR(TEXT("IMG_Init: Failed to init required jpg and png support!\n"));
			LOG_ERROR(TEXT("IMG_Init: "), IMG_GetError());
			return false;
		}
		atexit(IMG_Quit);

		
		//LOG_FUNKY(fk::getExecPath());

		// IMGUI
		LOG("Init ImGUI");
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hWnd);
		// IMGUI END

		LOG("Init Rendering");
		if (!RenderingBackend.Init(hWnd))
			return false;

		LOG("Create Renderer");
		Renderer = new Rendering::Renderer(RenderingBackend);
		LOG("Init Renderer");
		Renderer->InitBuffers();

		#ifdef FUNKY_EDITOR
			LOG("Create Editor Context");
			Editor = new Editor::EditorContext();

			LOG("Init Editor Context");
			if (!Editor->Init())
				return false;
		#endif // FUNKY_EDITOR


		return true;
	}

	void FunkyEngine::Run()
	{
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
				struct 
				{
					Math::Camera* Camera = nullptr;
					Scene* RelevantScene = nullptr;

					bool IsValid() const { return Camera != nullptr && RelevantScene != nullptr; }
				} View;

				#ifdef FUNKY_EDITOR
					Editor->Update();
				
					View.Camera = &Editor->MainCamera;
					View.RelevantScene = &MainScene;
				#endif // FUNKY_EDITOR

				RenderingBackend.BindDefaultRenderTarget();
				RenderingBackend.ClearRenderTargetWithColor({ 0.392156899f, 0.584313750f, 0.929411829f });
				RenderingBackend.ClearDepthStencil(1.0f, 0u);
				RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);

				if(View.IsValid())
					//Renderer->DrawSceneFromView(View.Camera, View.RelevantScene);
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

	bool FunkyEngine::CreateAndShowWindow(Math::Vec2u const & windowSize)
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
			LOG_ERROR(TEXT("couldn't register a class with name "), WND_CLASS_NAME);
			return false;
		}

		hWnd = CreateWindow(WND_CLASS_NAME, TEXT("Window name"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowSize.X, windowSize.Y, NULL, NULL, hInstance, NULL);
		if (!hWnd)
		{
			BREAK();
			LOG_ERROR(TEXT("couldn't create the window with class: "), WND_CLASS_NAME);
			UnregisterClass(WND_CLASS_NAME, hInstance);
			return false;
		}

		ShowWindow(hWnd, SW_SHOW);

		return true;
	}

}