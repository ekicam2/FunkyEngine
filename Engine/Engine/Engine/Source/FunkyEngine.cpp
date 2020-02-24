#include "FunkyEngine.h"

#include "DebugMacros.h"

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

#include "BasicTypes.h"

#include "Math/Vector2.h"
#include "Math/Camera.h"

#include "RenderingFrontend/Renderer.h"

#include "Core/Containers.h"
#include "Core/String.h"

#include "Utils/MeshUtils.h"

#include "Core/Tasks/ITask.h"
#include "Core/Timer.h"

#include "SDL.h"

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return Funky::FunkyEngine::GetEngine()->ProcessInput(hWnd, Message, wParam, lParam);
}

namespace Funky
{
	FunkyEngine* FunkyEngine::_Engine = nullptr;
	Core::IO::IIOSystem* FunkyEngine::_IO = nullptr;

	FunkyEngine::FunkyEngine()
		: RenderingBackend()
		, IOSystem(new Core::IO::WindowsIOSystem())
	{
		FunkyEngine::_Engine = this;
		FunkyEngine::_IO = IOSystem;

	}

	FunkyEngine::~FunkyEngine()
	{
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

	LRESULT CALLBACK FunkyEngine::ProcessInput(HWND InhWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
			
		// sort through and find what code to run for the message given
		switch (Message)
		{
		case WM_SIZE:
		{
			if(RenderingBackend && RenderingBackend->IsInitialized())
				RenderingBackend->OnViewportResized({ LOWORD(lParam), HIWORD(lParam) });
			return 0;
		}
		break;
		case WM_DESTROY:
		{
			bShouldRun = false;
			// close the application entirely
			//PostQuitMessage(0);
			return 0;
		} break;
		}

		// Handle any messages the switch statement didn't
		return DefWindowProc(InhWnd, Message, wParam, lParam);
	}

	bool FunkyEngine::Init()
	{
		 ThreadPool.Reset(new Core::Thread::ThreadPool({ {Core::Thread::Group::Rendering, (u16)1u}, {Core::Thread::Group::Worker, (u16)5u} }));
		 TaskManager.Reset(new Core::Task::TaskManager(ThreadPool));

		AssetManager.Reset(new Funky::AssetRegistry());

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
		

		LOG("Init SDL IMAGE");
		int flags = IMG_INIT_JPG | IMG_INIT_PNG;
		int initted = IMG_Init(flags);
		if ((initted&flags) != flags)
		{
			LOG_ERROR(TEXT("IMG_Init: Failed to init required jpg and png support!\n"));
			LOG_ERROR(TEXT("IMG_Init: "), IMG_GetError());
			return false;
		}
		
		//LOG_FUNKY(fk::getExecPath());
		TaskManager->Tick();
		LOG("Init Rendering");
		Rendering::DX11RenderingInitDesc InitDesc;
		InitDesc.Api = Rendering::RenderingBackend::API::DX11;
		InitDesc.hWnd = hWnd;
		
		RenderingBackend.Reset(new Rendering::RenderingBackend());
		if (!RenderingBackend->Init(&InitDesc))
			return false;

		LOG("Create Renderer");
		Renderer.Reset(new Rendering::Renderer(*RenderingBackend));
		LOG("Init Renderer");
		if (!Renderer->Init())
		{
			LOG_ERROR("Couldn't initialize renderer!");
			return false;
		}

		LOG("Initializing SceneManager");
		MainSceneManager.Reset(new SceneManager());

		return true;
	}

	void FunkyEngine::Run()
	{
		bool bGotMsg;

		MSG  Msg;
		Msg.message = WM_NULL;
		Timer FrameTimer;
		while (bShouldRun)
		{
			TaskManager->Tick();

			float DeltaTime = (float)FrameTimer.Reset(Timer::EResolution::Mills);

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
				MainSceneManager->Tick(DeltaTime);
				RenderScene();
			}
		}
	}

	bool FunkyEngine::Shutdown()
	{
		LOG("FunkyEngine::Shutdown()");


		LOG("\tExiting SDL");
		SDL_Quit();
		LOG("\tExiting SDL Image");
		IMG_Quit();

		LOG("Shutdown Completed!");
		return true;
	}

	void FunkyEngine::RenderScene()
	{
		class RenderTask : public Core::Task::ITask
		{
		public:
			RenderTask(Rendering::IRenderer* InRenderer) 
			{ 
				SetThreadGroup(Core::Thread::Group::Rendering); Renderer = InRenderer; 
			}
			virtual void Process() override 
			{ 
				Renderer->DrawScene(nullptr); 
			}

			Rendering::IRenderer* Renderer;
		};

		TaskManager->EnqueueTask(new RenderTask(Renderer));

		//Renderer->DrawScene(nullptr /* CurrentScene->CreateDrawScene()*/);
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