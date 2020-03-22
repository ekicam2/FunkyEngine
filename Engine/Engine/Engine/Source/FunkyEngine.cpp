#include "FunkyEngine.h"

#include "DebugMacros.h"

#include "BasicTypes.h"

#include "Math/Vector2.h"
#include "Math/Camera.h"

#include "RenderingFrontend/Renderer.h"

#include "Core/Containers.h"
#include "Core/String.h"

#include "Utils/MeshUtils.h"

#include "Core/Tasks/ITask.h"
#include "Core/Timer.h"

#define THREADED_RENDERING 0

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return Funky::FunkyEngine::GetEngine()->ProcessInput(hWnd, Message, wParam, lParam);
}

namespace Funky
{

	FunkyEngine* FunkyEngine::_Engine = nullptr;

	FunkyEngine::FunkyEngine()
		: IOSystem(new Core::IO::WindowsIOSystem())
	{
		FunkyEngine::_Engine = this;

	}

	FunkyEngine::~FunkyEngine()
	{
	}

	Core::IO::IIOSystem* FunkyEngine::GetIO()
	{
		CHECK(_Engine->IOSystem);
		return _Engine->IOSystem;
	}

	FunkyEngine* FunkyEngine::GetEngine()
	{
		CHECK(_Engine);
		return _Engine;
	}

#pragma region OSDependent
	LRESULT CALLBACK FunkyEngine::ProcessInput(HWND InhWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
			
		// sort through and find what code to run for the message given
		switch (Message)
		{
		case WM_SIZE:
		{
			if(RenderingBackend && RenderingBackend->IsInitialized())
				RenderingBackend->OnViewportResized({ LOWORD(lParam), HIWORD(lParam) });
				//RenderingBackend->OnViewportResized({ 2560, 1080 });
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
#pragma endregion

	bool FunkyEngine::Init()
	{
#if THREADED_RENDERING
		 ThreadPool.Reset(new Core::Thread::ThreadPool({ {Core::Thread::Group::Rendering, (u16)1u}, {Core::Thread::Group::Worker, (u16)2u} }));
		 TaskManager.Reset(new Core::Task::TaskManager(ThreadPool));
#endif
		
		 //AssetManager.Reset(new Funky::AssetRegistry());

		constexpr u32 uWindowWidth = 2048u;
		constexpr u32 uWindowHeight = 1024u;

		LOG("Creating widnow with size: ", uWindowWidth, "x", uWindowHeight);

		if (!CreateAndShowWindow({ uWindowWidth, uWindowHeight }))
			return false;

		//LOG_FUNKY(fk::getExecPath());
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

		LOG("Initializing MainScene");
		InitScene();
		auto CurrentScene = MainSceneManager->GetCurrentScene();
		CurrentScene->Init();

		return true;
	}

	void FunkyEngine::Run()
	{
#pragma region OSDependent
		bool bGotMsg;

		MSG  Msg;
		Msg.message = WM_NULL;
		Timer FrameTimer;
#pragma endregion
		while (bShouldRun)
		{
#if THREADED_RENDERING
			TaskManager->Tick();
#endif


			float DeltaTime = (float)FrameTimer.Reset(Timer::EResolution::Mills);
#pragma region OSDependent
			bGotMsg = (PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE) != 0);
			if (bGotMsg)
			{
				// Translate and dispatch the message
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);

				IOSystem->Update();
			}
			else
#pragma endregion 
			{
				MainSceneManager->Tick(DeltaTime);
				RenderScene();
			}
		}
	}

	bool FunkyEngine::Shutdown()
	{
		LOG("FunkyEngine::Shutdown()");

		LOG("Shutdown Completed!");
		return true;
	}

	void FunkyEngine::RenderScene()
	{
#if THREADED_RENDERING
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
#else
		/* sync point*/
		Core::Memory::UniquePtr<Rendering::RenderScene> Scene(Renderer->CreateRenderScene(MainSceneManager->GetCurrentScene()));
		/* sync point end */
		Renderer->DrawScene(Scene);
#endif

	}
#pragma region OSDependent
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
#pragma endregion

	void FunkyEngine::InitScene()
	{

	}

}