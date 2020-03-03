#pragma once

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

namespace Funky
{
	class FunkyEngine
	{
	public:
		FunkyEngine();
		~FunkyEngine();
		static Core::IO::IIOSystem* GetIO();
		static FunkyEngine* GetEngine();
		
		LRESULT __stdcall ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool Init();
		void Run();
		bool Shutdown();

		void RenderScene();

		AssetRegistry* GetAssetRegistry() { return AssetManager; }
		Rendering::RenderingBackend* GetRenderingBackend() { return RenderingBackend.Get(); }

	private:
		bool CreateAndShowWindow(Math::Vec2u const & windowSize);
		bool bShouldRun = true;

		void InitScene();

		static FunkyEngine* _Engine;

		Core::Memory::UniquePtr<Rendering::RenderingBackend> RenderingBackend;
		Core::Memory::UniquePtr<Rendering::IRenderer> Renderer;

		Core::Memory::UniquePtr<Core::IO::IIOSystem> IOSystem;

		Core::Memory::UniquePtr<Core::Task::TaskManager> TaskManager;
		Core::Memory::UniquePtr<Core::Thread::ThreadPool> ThreadPool;

		Core::Memory::UniquePtr<AssetRegistry> AssetManager;

		Core::Memory::UniquePtr<SceneManager> MainSceneManager;

		HWND hWnd;
		HINSTANCE hInstance = GetModuleHandle(NULL);
	};
}