#pragma once

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"
#include "RenderingFrontend/RenderScene.h"

#include <optional>
#include <fstream>

#include "Macros.h"

namespace Funky
{
	namespace Asset
	{
		struct Material : public IAsset
		{
			static Material* CreateMaterialFromSourceCode(char const* VSSource, char const* PSSource)
			{
				Material* Ret = new Material();
				Ret->VSSource = (VSSource);
				Ret->PSSource = (PSSource);
				return Ret;
			}

			enum class RenderingTechnique
			{
				DefaultLit,
				Internal_Depth
			};

			static std::optional<std::pair<str, str>> ParseMaterial(str const& Path);

			FORCEINLINE char const * GetVertexShaderSourceCode() const { return VSSource.c_str(); }
			FORCEINLINE char const * GetPixelShaderSourceCode() const { return PSSource.c_str(); }

			FORCEINLINE size GetVertexShaderSourceCodeLength() const {
				return strlen(GetVertexShaderSourceCode());
			}

			FORCEINLINE size GetPixelShaderSourceCodeLength() const {
				return strlen(GetPixelShaderSourceCode());
			}
			//todo cleanup
		private:
			str VSSource;
			str PSSource;
		public:
			RenderingTechnique Technique = RenderingTechnique::DefaultLit;

			Rendering::ShaderLink Linkage;
		};
	}
}
