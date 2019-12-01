#pragma once

#ifdef FUNKY_EDITOR

#include "Editor/WindowManager.h"

#include "Math/Camera.h"

#include "Core/Assets/RawMesh.h"

namespace Funky
{
	namespace Editor
	{
		class EditorContext : public IWindow
		{
		public:
			EditorContext();
			bool Init();

			virtual void DrawGUI() override;

			void Update();

			Math::Camera MainCamera;
		protected:
			bool bPrevFrameRMB;


			f32 CameraSpeed = 0.5f;
			f32 CameraX = 0.75f;
			f32 CameraY = 0.75f;


			std::unique_ptr<Asset::RawMesh> EditorCube;
			std::unique_ptr <Asset::RawMesh> EditorSphere;
		};
	}
}
#endif // FUNKY_EDITOR