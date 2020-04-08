#pragma once

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"
#include "RenderingBackend/RenderingBackend.h"
#include "RenderingFrontend/RenderView.h"

#include <optional>
#include <fstream>

#include "Macros.h"
#include "DebugMacros.h"

namespace Funky
{
	class ShaderCompiler;

	namespace Asset
	{
		struct Shader : public IAsset
		{
			friend class ShaderCompiler;

			enum EShaderType
			{
				Vertex,
				Pixel
			};

			[[nodiscard]] static Shader* CreateFromFile(EShaderType InType, Str const& InPath);
			[[nodiscard]] static Shader* CreateShaderFromSource(EShaderType InType, Str const& InSource);

			FORCEINLINE EShaderType GetType() const { return Type; }

			FORCEINLINE char const* GetSource() const { return Source.GetBuffer(); }
			FORCEINLINE size GetSourceLength() const { return Source.Length(); }

			FORCEINLINE byte* GetBuffer() const { return CompiledBuffer.Get(); }
			FORCEINLINE size GetBufferSize() const { return BufferSizeInBytes; }
			FORCEINLINE bool IsValid() const { return bIsCompiled; }

		private:
			EShaderType Type;

			Str Source;

			size BufferSizeInBytes;
			Core::Memory::UniquePtr<byte[]> CompiledBuffer;
			bool bIsCompiled = false;

			DECLARE_IASSET(Shader, Asset::EAssetType::Shader);
		};

		struct Material : public IAsset
		{
			[[nodiscard]] static Material* CreateMaterial(Funky::Asset::Shader* VS, Funky::Asset::Shader* PS);

			enum class ERenderingTechnique
			{
				Toon,
				DefaultLit
			} Technique;


			FORCEINLINE Shader* GetPS() const { CHECK(PS != nullptr && PS->GetType() == Shader::EShaderType::Pixel); return PS; }
			FORCEINLINE Shader* GetVS() const { CHECK(VS != nullptr && VS->GetType() == Shader::EShaderType::Vertex); return VS; }

			FORCEINLINE bool IsValid() const { return PS != nullptr && VS != nullptr && PS->GetType() == Shader::EShaderType::Pixel && VS->GetType() == Shader::EShaderType::Vertex; }

		private:
			DECLARE_IASSET(Material, Asset::EAssetType::Material);

			Shader* VS = nullptr;
			Shader* PS = nullptr;

		public:
			Rendering::ShaderLink Linkage;
		};
	}
}
