#pragma once

#ifdef FUNKY_EDITOR

#include "Macros.h"

#include "Core/Containers.h"
#include "Core/String.h"

namespace Funky
{
	namespace Editor
	{
		/* At some point I will change this implementation however for now it can stay this way. */
		class IWindow
		{
		public:
			IWindow(str const & WindowName);
			virtual ~IWindow();

			virtual void DrawGUI() = 0;

			FORCEINLINE void ShowGUI() { Visible = true; }
			FORCEINLINE void HideGUI() { Visible = false; }

			FORCEINLINE bool IsVisible() const { return Visible; }

			FORCEINLINE str const& GetWindowName() const { return WindowName; }

		private:
			const str WindowName;
			bool Visible;
		};

		/* Special window which cannot be closed. */
		class WindowManagerUI
		{
		public:
			WindowManagerUI(class WindowManager* Parent);
			void DrawGUI();

		private:
			WindowManager* Parent = nullptr;
		};

		class WindowManager
		{
			friend class WindowManagerUI;

		public:
			WindowManager();

			void DrawGUI();

			void RegisterWindow(IWindow* Window);
			void UnregisterWindow(IWindow* Window);

		private:
			darray<IWindow*> AllWindows;
			WindowManagerUI* UIWindow;
		};

	}
}
#endif // FUNKY_EDITOR