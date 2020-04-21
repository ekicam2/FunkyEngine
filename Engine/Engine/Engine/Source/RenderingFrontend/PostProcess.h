#pragma once

#include "Core/Assets/Asset.h"

#include "RenderingBackend/RenderingBackend.h"
#include "RenderingFrontend/RenderPrimitive.h"

namespace Funky
{
	namespace Rendering
	{
		class PostProcess
		{
		public:
			static void InitPostProcessSurface(RenderingBackend* RB);
			static Asset::ID GetPostProcessSurface();

			static void InitVS(RenderingBackend* RB);
			static Asset::ID GetVSShader();

			static PostProcess* CreateFromSource(Str FilePath, RenderingBackend* RB);

		private:
			static Asset::ID PostProcessPlaneData;
			static Asset::ID UnifiedVertexShader;

			Asset::ID PostProcessImplementation;
		};

	}
}