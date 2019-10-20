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

namespace Funky
{
	//TODO(ekicam2): implment IOSystem
	struct IOSystem
	{
		enum class Key : unsigned
		{
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			R,
			S,
			T,
			U,
			W,
			X,
			Y,
			Z,

			ARROW_UP,
			ARROW_DOWN,
			ARROW_LEFT,
			ARROW_RIGHT,

			TAB,
			SPACE,

			COUNT
		};

		bool Keys[(unsigned)Key::COUNT];
	};

	class FunkyEngine
	{
	public:
		FunkyEngine();
		static IOSystem const & GetIO();
		static FunkyEngine* GetEngine();

		LRESULT __stdcall ProcessInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool Init();
		void Run();
		bool Shutdown();
		void DrawGUI();

		Rendering::RenderingBackend* GetRenderingBackend() { return &RenderingBackend; }

	private:
		bool CreateAndShowWindow(Math::Vector2u const & windowSize);

		static FunkyEngine* Engine;
		static IOSystem* IO;

		Rendering::RenderingBackend RenderingBackend;
		Rendering::Renderer* Renderer;
		Scene MainScene;

		Core::Task::TaskManager TaskManager;
		Core::Thread::ThreadPool ThreadPool;

		HWND hWnd;
		HINSTANCE hInstance = GetModuleHandle(NULL);

		//editor
		f32 CameraSpeed = 0.5f;
		f32 CameraX = 0.75f;
		f32 CameraY = 0.75f;
	};
}