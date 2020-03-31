#pragma once

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"
#include "RenderingFrontend/RenderView.h"

#include <optional>
#include <fstream>

#include "Macros.h"

namespace Funky
{
	namespace Asset
	{
		struct Material : public IAsset
		{
			static Material* CreateMaterialFromSourceCode(char const* VSSource, char const* PSSource);

			enum class ERenderingTechnique
			{
				DefaultLit,
				Internal_Depth
			} Technique;

			FORCEINLINE char const * GetVertexShaderSourceCode() const { return VSSource.c_str(); }
			FORCEINLINE char const * GetPixelShaderSourceCode() const { return PSSource.c_str(); }

			FORCEINLINE size GetVertexShaderSourceCodeLength() const { return strlen(GetVertexShaderSourceCode()); }
			FORCEINLINE size GetPixelShaderSourceCodeLength() const { return strlen(GetPixelShaderSourceCode()); }
			
		private:
			DECLARE_IASSET(Material, Asset::EType::Material)

			str VSSource;
			str PSSource;
		public:
			Rendering::ShaderLink Linkage;
		};
	}
}
