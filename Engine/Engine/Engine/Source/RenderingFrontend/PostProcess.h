#pragma once

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
			static RBuffer* GetPostProcessSurface();

			static void InitVS(RenderingBackend* RB);
			static RShader* GetVSShader();

			static PostProcess* CreateFromSource(Str FilePath, RenderingBackend* RB);

			ShaderLink GetShaderLinkage() const;

		private:
			static RBuffer*	PostProcessPlaneData;
			static RShader* UnifiedVertexShader;

			RShader* PostProcessImplementation;
		};

	}
}