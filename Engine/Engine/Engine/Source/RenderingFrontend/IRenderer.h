#pragma once

#include "RenderingBackend/RenderingBackend.h"

namespace Funky
{
	struct IScene;

	namespace Rendering
	{
		struct RenderView;

		class IRenderer
		{
		public:
			IRenderer(Rendering::RenderingBackend& RenderingBackend) : RenderingBackend(RenderingBackend) {}
			virtual ~IRenderer() = default;

			virtual bool Init() = 0;
			virtual void Shutdown() = 0;

			virtual RenderView* CreateRenderScene(IScene* Scene) = 0;
			virtual void DrawScene(RenderView* SceneToRender) = 0;

		protected:
			Rendering::RenderingBackend& RenderingBackend;
		};

	}
}

#define FUNKY_DEFINE_RENDERER_CONSTRUCTOR(__name__) \
__name__(Rendering::RenderingBackend& RenderingBackend) : IRenderer(RenderingBackend) {}