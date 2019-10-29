#pragma once

#include "Editor/WindowManager.h"

#include "Math/Camera.h"

namespace Funky
{
	namespace Editor
	{
		class EditorContext : public IWindow
		{
		public:
			EditorContext();
			virtual void DrawGUI() override;

			void Update();

			Math::Camera MainCamera;
		protected:
			bool bPrevFrameRMB;


			f32 CameraSpeed = 0.5f;
			f32 CameraX = 0.75f;
			f32 CameraY = 0.75f;
		};
	}
}