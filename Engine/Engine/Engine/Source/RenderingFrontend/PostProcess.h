#pragma once

#include "RenderingBackend/RenderingBackend.h"

namespace Funky
{
	namespace Rendering
	{
		class PostProcess
		{
		public:
			static void InitPostProcessSurface(RenderingBackend* RB);
			static RBuffer* GetPostProcessSurface();

			static void InitVS();
			static RShader* GetVSShader();

			static PostProcess* CreateFromSource(str FilePath);

		private:
			static RBuffer* PostProcessPlaneData;
			static RShader* UnifiedVertexShader;

			RShader* PostProcessImplementation = nullptr;
		};

	}
}