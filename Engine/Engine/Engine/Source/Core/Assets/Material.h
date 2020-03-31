#pragma once

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"
#include "RenderingBackend/RenderingBackend.h"
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
				Toon,
				DefaultLit
			} Technique;

			FORCEINLINE char const * GetVertexShaderSourceCode() const { return VSSource.c_str(); }
			FORCEINLINE char const * GetPixelShaderSourceCode() const { return PSSource.c_str(); }

			FORCEINLINE size GetVertexShaderSourceCodeLength() const { return strlen(GetVertexShaderSourceCode()); }
			FORCEINLINE size GetPixelShaderSourceCodeLength() const { return strlen(GetPixelShaderSourceCode()); }

			FORCEINLINE byte* GetVSBuffer() const { return CompiledVS.Get(); }
			FORCEINLINE size GetVSBufferSize() const { return CompiledVSSize; }

			FORCEINLINE byte* GetPSBuffer() const { return CompiledPS.Get(); }
			FORCEINLINE size GetPSBufferSize() const { return CompiledPSSize; }
			
			FORCEINLINE bool IsCompiled() const { return bIsCompiled; }

			void Compile(Rendering::RenderingBackend::API BackendApi);
			
		private:
			DECLARE_IASSET(Material, Asset::EType::Material);

			str VSSource;
			str PSSource;

			bool bIsCompiled = false;

			Core::Memory::UniquePtr<byte[]> CompiledVS;
			size CompiledVSSize = 0u;

			Core::Memory::UniquePtr<byte[]> CompiledPS;
			size CompiledPSSize = 0u;

		public:
			Rendering::ShaderLink Linkage;
		};
	}
}
