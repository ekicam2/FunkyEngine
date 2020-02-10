#pragma once

#include "RenderingBackend/RenderingBackend.h"

namespace Funky
{
	namespace Rendering
	{
		class IRenderer
		{
		public:
			IRenderer(Rendering::RenderingBackend& RenderingBackend) : RenderingBackend(RenderingBackend) {}
			virtual ~IRenderer() = default;

			virtual bool Init() = 0;
			virtual void Shutdown() = 0;

			virtual void DrawScene(class RenderScene* SceneToRender) = 0;

		protected:
			Rendering::RenderingBackend& RenderingBackend;
		};
	}
}

#define FUNKY_DECLARE_RENDERER_CONSTRUCTOR(__name__) \
__name__(Rendering::RenderingBackend& RenderingBackend) : IRenderer(RenderingBackend) {}