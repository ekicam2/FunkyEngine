#include "Engine.h"

#include "DebugMacros.h"

#include "BasicTypes.h"

#include "Math/Vector2.h"
#include "Math/Camera.h"

#include "RenderingFrontend/Renderer.h"

#include "Core/Containers.h"
#include "Core/String.h"

#include "Utils/MeshUtils.h"

#include "Core/Utils.h"
#include "Core/Tasks/ITask.h"
#include "Core/Timer.h"

LRESULT CALLBACK ProcessInput(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return Funky::Engine::GetEngine()->ProcessInput(hWnd, Message, wParam, lParam);
}

namespace Funky
{

	Engine* Engine::_Engine = nullptr;

	Engine::Engine()
		: IOSystem(new Core::IO::WindowsIOSystem())
	{
		Engine::_Engine = this;

	}

	Core::IO::IIOSystem* Engine::GetIO()
	{
		CHECK(_Engine->IOSystem);
		return _Engine->IOSystem;
	}

	Engine* Engine::GetEngine()
	{
		CHECK(_Engine);
		return _Engine;
	}

#pragma region OSDependent
	LRESULT CALLBACK Engine::ProcessInput(HWND InhWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
			
		// sort through and find what code to run for the message given
		switch (Message)
		{
		case WM_SIZE:
		{
			Funky::OnViewportResized.Broadcast({ LOWORD(lParam), HIWORD(lParam) });
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


bool Engine::Init(i32 Argc, char** Argv)
{
		ParseProgramArgs(Argc, Argv);

		// load from engine .ini or .json IDC <3
		SubsystemBitmask RequestedSubsystems = SubsystemBitmask::All;
		return InitSubsystems(RequestedSubsystems);
	}

	void Engine::Run()
	{
		// CollectKickOffTasks
		// Dispatch CollectedTasks

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

	bool Engine::Shutdown()
	{
		LOG("FunkyEngine::Shutdown()");

		LOG("Shutdown Completed!");
		return true;
	}

	void Engine::RenderScene()
	{
#if THREADED_RENDERING
		class RenderTask : public Core::Task::ITask
		{
		public:
			RenderTask(Rendering::IRenderer* InRenderer) 
			{ 
				SetThreadGroup(Core::Thread::EGroup::Rendering); Renderer = InRenderer; 
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
		//Core::Memory::UniquePtr<Rendering::RenderView> Scene(Renderer->CreateRenderScene(MainSceneManager->GetCurrentScene()));
		/* sync point end */
		Renderer->DrawScene(MainSceneManager->GetCurrentScene());
#endif

	}
#pragma region OSDependent

	void Engine::ParseProgramArgs(i32 Argc, char** Argv)
	{
		DEBUG_SCOPE_TIMER(TEXT("Program arguments parsing "));

		if (Argc == 1)
		{
			LOG("No arguments to parse.");
			return;
		}

		EngineArguments::GetInstance().Init(Argc, Argv);
	}

	bool Engine::InitSubsystems(SubsystemBitmask Expected)
	{
		SubsystemBitmask Result = SubsystemBitmask::None;

#if THREADED_RENDERING
		ThreadPool.Reset(new Core::Thread::ThreadPool({ {Core::Thread::EGroup::Rendering, (u16)1u}, {Core::Thread::EGroup::Worker, (u16)2u} }));
		TaskManager.Reset(new Core::Task::TaskManager(ThreadPool));
#endif
		if (Expected & SubsystemBitmask::AssetManagement)
		{
			AssetManager.Reset(new Funky::AssetRegistry());
			Result |= SubsystemBitmask::AssetManagement;
		}

		// Window subsystem
		if(Expected & SubsystemBitmask::Window)
		{
			const u32 uWindowWidth = EngineArguments::GetInstance().Find("-windowX", 2048);
			const u32 uWindowHeight = EngineArguments::GetInstance().Find("-windowY", 1024);

			if (!CreateAndShowWindow({ uWindowWidth, uWindowHeight }))
			{
				LOG_ERROR("Coudln't create window");
			}
			else
			{
				Result |= SubsystemBitmask::Window;
			}
		}

		// Rendering API
		{
			Rendering::RenderingBackend::RenderingBackendInitResult CreationResult;

			if (Expected & SubsystemBitmask::Rendering)
			{
				LOG("Init Rendering");
				const auto RenderingBackendAPI = static_cast<Rendering::RenderingBackend::EAPI>(EngineArguments::GetInstance().Find("-dx11", 1));
				if (RenderingBackendAPI == Rendering::RenderingBackend::EAPI::DX11)
				{
		
					Rendering::DX11RenderingInitDesc InitDesc;
					InitDesc.Api = Rendering::RenderingBackend::EAPI::DX11;
					InitDesc.hWnd = hWnd;
		
					RenderingBackend.Reset(new Rendering::RenderingBackend());
					if (RenderingBackend->Init(&InitDesc, &CreationResult))
						Result |= SubsystemBitmask::Rendering;
				}
				else
				{
					LOG_ERROR("Couldn't create rendering backend");
				}
			}

			// Renderer
			if (Expected & SubsystemBitmask::Renderer)
			{
				LOG("Create Renderer");
				Renderer.Reset(new Rendering::Renderer(*RenderingBackend));
				LOG("Init Renderer");
				if (!Renderer->Init(&CreationResult))
				{
					LOG_ERROR("Couldn't initialize renderer!");
				}
				else
				{
					Result |= SubsystemBitmask::Renderer;
				}
			}
		}

		// Scene management
		if (Expected & SubsystemBitmask::SceneManagement)
		{
			LOG("Initializing SceneManager");
			MainSceneManager.Reset(new SceneManager());

			LOG("Initializing MainScene");
			InitScene();

			Result |= SubsystemBitmask::SceneManagement;

		}

		return Result == Expected;
	}

	bool Engine::CreateAndShowWindow(Math::Vec2u const& windowSize)
	{
		WindowSize = windowSize;

		LOG("Creating widnow with size: ", windowSize.X, "x", windowSize.Y);

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

	void Engine::InitScene()
	{
		auto CurrentScene = MainSceneManager->GetCurrentScene();
		CurrentScene->Init();
	}

	Funky::SubsystemBitmask& operator|=(SubsystemBitmask& Lhs, SubsystemBitmask Rhs)
	{
		Lhs = static_cast<SubsystemBitmask>(static_cast<u8>(Lhs) | static_cast<u8>(Rhs));
		return Lhs;
	}

	Funky::SubsystemBitmask& operator&=(SubsystemBitmask& Lhs, SubsystemBitmask Rhs)
	{
		Lhs = static_cast<SubsystemBitmask>(static_cast<u8>(Lhs) & static_cast<u8>(Rhs));
		return Lhs;
	}

	bool operator&(SubsystemBitmask Lhs, SubsystemBitmask Rhs)
	{
		return static_cast<u8>(Lhs) & static_cast<u8>(Rhs);
	}
}