#pragma once

#include "Core/Events/EngineEvents.h"

#include "Core/Platform/Platform.h"
#include "Core/Memory/UniquePtr.h"

#include "Math/Vector2.h"

#include "RenderingBackend/RenderingBackend.h"
#include "RenderingFrontend/IRenderer.h"

#include "Core/Tasks/TaskManager.h"
#include "Core/Thread/ThreadPool.h"

#include "Core/Assets/AssetRegistry.h"

#include "Core/IIOSystem.h"

#include "UserFramework/SceneManager.h"
#include "Core/Utils.h"

namespace Funky
{
	/** Used in @EngineArguments Read Oly set.*/
	struct ARGValue
	{
		union
		{
			i32 Int;
		};

		enum class EType
		{
			OccurenceSwitch,
			Integer
		} Type = EType::OccurenceSwitch;

#ifdef _DEBUG
		Str DebugName;
#endif
	};

	/** Passed to the init method so we know what to initialize and what to not. */
	enum class SubsystemBitmask : u8
	{
		None			= 0,
		Window			= (1 << 0),
		Rendering		= (1 << 1),
		Renderer		= (1 << 2),
		SceneManagement = (1 << 3),
		AssetManagement = (1 << 4),
		All				= Window | Rendering | Renderer | SceneManagement | AssetManagement
	};

	SubsystemBitmask& operator|=(SubsystemBitmask& Lhs, SubsystemBitmask Rhs);

	SubsystemBitmask& operator&=(SubsystemBitmask& Lhs, SubsystemBitmask Rhs);

	bool operator&(SubsystemBitmask Lhs, SubsystemBitmask Rhs);

	/** Main class of the engine, at some point it will be replaced with more static functions, or set of singleton getters.*/
	class Engine final
	{
		struct EngineArguments
		{
			static EngineArguments& GetInstance()
			{
				static EngineArguments Instance;
				return Instance;
			}

			template<typename T = i32>
			T Find(Str const& Input, T const & Default = T(0)) const
			{
				auto found = Args.find(HashString(Input));
				if (found != Args.end() && found->second.Type == ARGValue::EType::Integer)
					return found->second.Int;

				return Default;
			}
#ifdef _DEBUG
			void DumpFormatted()
			{
				auto it = Args.cbegin();
				size i = 0;
				while (it != Args.cend())
				{
					LOG(i, ": ", (*it).second.DebugName, " ", ((*it).second.Type == ARGValue::EType::OccurenceSwitch ? " occurence " : " integer "), ((*it).second.Type == ARGValue::EType::OccurenceSwitch ? 1 : (*it).second.Int));

					it++;
					i++;
				}
			}
#endif

		private:
			friend class Engine;
			void Init(i32 Argc, char** Argv)
			{
				ARGValue* Prev = nullptr;

				for (i32 i = 1; i < Argc; ++i)
				{
					Str ar = Argv[i];
					if (ar.StartsWith("-"))
					{
						ARGValue Value;
#ifdef _DEBUG
						Value.DebugName = ar;
#endif

						auto it = Args.insert({ HashString(ar), Move(Value) });
						Prev = &((*it.first).second);
					}
					else
					{
						Prev->Int = std::atoi(ar.GetBuffer());
						Prev->Type = ARGValue::EType::Integer;
					}
				}
			}
			map<Hash128, ARGValue> Args;
		};

	public:
		ENGINE_MODULE Engine();
		void								ENGINE_MODULE RequestExit() { bShouldRun = false; }

		static Core::IO::IIOSystem const *	ENGINE_MODULE GetIO();
		static Engine*						ENGINE_MODULE GetEngine(); //!< This one should be replaced
		
		SceneManager* ENGINE_MODULE GetSceneManager() const { return MainSceneManager;}


		/** These below should be moved to the Platform::Window */
		FORCEINLINE Math::Vec2u ENGINE_MODULE GetWindowSize()	const { return WindowSize; }
		FORCEINLINE HWND		ENGINE_MODULE GetHwnd()			const { return hWnd; }
		/** END */

		struct EngineInitDesc
		{
			i32 Argc;
			char** Argv;
			SubsystemBitmask RequiredSubsystems = SubsystemBitmask::All;
			Core::Event<void(Engine*, bool)> Initializer;
		};
		bool ENGINE_MODULE Init(EngineInitDesc* initDesc);
		void ENGINE_MODULE Run();
		bool ENGINE_MODULE Shutdown();

		/** What is scene, what is rendering. Move to the game itself I DONT GIVE A FUCK.*/
		void RenderScene();

		/** This one should be independent from Engine state. */
		AssetRegistry* GetAssetRegistry() { return AssetManager; }

		/** */
		Rendering::RenderingBackend* GetRenderingBackend() { return RenderingBackend.Get(); }

		/** This one belongs to the Platform::IO */
		LRESULT __stdcall ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		void ParseProgramArgs(i32 Argc, char** Argv);
		[[nodiscard]]
		bool InitSubsystems(SubsystemBitmask Expected);

		[[nodiscard]]
		bool CreateAndShowWindow(Math::Vec2u const& windowSize);
		bool bShouldRun = true;

		static Engine* _Engine;

		// subsystems
		Core::Memory::UniquePtr<Core::IO::IIOSystem> IOSystem;

		Core::Memory::UniquePtr<AssetRegistry> AssetManager;

#if THREADED_RENDERING
		Core::Memory::UniquePtr<Core::Task::TaskManager> TaskManager;
			Core::Memory::UniquePtr<Core::Thread::ThreadPool> ThreadPool;
#endif
		Core::Memory::UniquePtr<SceneManager> MainSceneManager;
			
		Core::Memory::UniquePtr<Rendering::IRenderer> Renderer;
			Core::Memory::UniquePtr<Rendering::RenderingBackend> RenderingBackend;
		// subsystems END

		/** These below should be moved to the Platform::Window */
		Math::Vec2u WindowSize = Math::Vec2u::Zero;
		HWND hWnd;
		HINSTANCE hInstance = GetModuleHandle(NULL);
		/** END */
	};
}