#pragma once

#include "Core/Platform/Platform.h"

#define ENDABLE_LOG
#define LOG_EVERYWHERE
#include "LogMacros.h"

#include "Math/Vector2.h"

#include "Rendering/RenderingBackend.h"

#include "Rendering/Scene.h"
#include "Rendering/Renderer.h"

#include "Core/Tasks/TaskManager.h"
#include "Core/Thread/ThreadPool.h"

#include "Core/Assets/AssetManager.h"

#include "Core/IIOSystem.h"

#include "Editor/WindowManager.h"

#include "Editor/EditorContext.h"

namespace Funky
{
	class FunkyEngine
	{
	public:
		FunkyEngine();
		~FunkyEngine();
		static Core::IO::IIOSystem* GetIO();
		static FunkyEngine* GetEngine();
		static Editor::WindowManager* GetEditorWindowManager();

		LRESULT __stdcall ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool Init();
		void Run();
		bool Shutdown();
		void DrawGUI();

		Rendering::RenderingBackend* GetRenderingBackend() { return &RenderingBackend; }

	private:
		bool CreateAndShowWindow(Math::Vector2u const & windowSize);

		static FunkyEngine* _Engine;
		static Core::IO::IIOSystem* _IO;

		Rendering::RenderingBackend RenderingBackend;
		Rendering::Renderer* Renderer;
		Scene MainScene;

		Core::IO::IIOSystem* IOSystem;

		Core::Task::TaskManager TaskManager;
		Core::Thread::ThreadPool ThreadPool;

		AssetManager AssetManager;

		HWND hWnd;
		HINSTANCE hInstance = GetModuleHandle(NULL);

		//editor
		Editor::WindowManager* EditorWindowManager;
		Editor::EditorContext* Editor;

		friend class Funky::Editor::EditorContext;
	};
}