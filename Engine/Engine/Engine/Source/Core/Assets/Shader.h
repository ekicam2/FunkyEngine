#pragma once

#include "Core/Utils.h"
#include "Core/String.h"
#include "RenderingFrontend/IRenderingResource.h"

namespace Funky
{
	class ShaderCompiler;
	class AssetRegistry;

	namespace Asset
	{
		struct Shader
		{
			friend class Funky::ShaderCompiler;
			friend class Funky::AssetRegistry;

			enum class EShaderType : u8
			{
				None,
				Vertex,
				Fragment
			};
			const static Str ShaderTypeToString[]; 

			FORCEINLINE EShaderType GetType() const { return Type; }

			FORCEINLINE char const* GetSource() const { return Source.GetBuffer(); }
			FORCEINLINE size GetSourceLength() const { return Source.Length(); }

			FORCEINLINE byte* GetBuffer() const { return CompiledBuffer.Get(); }
			FORCEINLINE size GetBufferSize() const { return BufferSizeInBytes; }
			FORCEINLINE bool IsValid(EShaderType expectedShader = EShaderType::None) const { return (expectedShader == EShaderType::None) ? Type != EShaderType::None : Type == expectedShader; }
			FORCEINLINE bool IsCompiled() const { return bIsCompiled; }

			Rendering::Resource::ID ShaderHandle = Rendering::Resource::ID::Zero;
			FORCEINLINE Hash128 GetHash() const { return HashString(Path + ShaderTypeToString[static_cast<u8>(Type)]); }

		protected:
			EShaderType Type;
			Str Path;
			Str Source;

			size BufferSizeInBytes;
			Core::Memory::UniquePtr<byte[]> CompiledBuffer;
			bool bIsCompiled = false;

			[[nodiscard]]
			static Shader* Create(Str const& path, EShaderType type);
		};

		inline const Str Shader::ShaderTypeToString[] = { "None", "Vertex", "Fragment" };

	}
}