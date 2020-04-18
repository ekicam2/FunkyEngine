#pragma once

#include "RenderingBackend/RenderingBackend.h"
#include "Core/Assets/Material.h"
#include "Core/Memory/Memory.h"


#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include <wrl/client.h>


//TODO: Assets in a template manner

namespace
{
	const Str ShadersDefaultPath = "";
}

namespace Funky
{
	class ShaderCompiler
	{
	public:
		struct ShaderDesc
		{
			Rendering::RenderingBackend::EAPI Api;

			Asset::Shader* ShaderAsset;

			struct ShaderMacro
			{
				Str Name;
				Str Value;
			};

			size MacrosCount = 0;
			Core::Memory::UniquePtr<ShaderMacro[]> Macros;

			Str IncludePath = "G:\\Engine\\Engine\\Engine\\Engine\\Work\\RealData\\Shaders\\Source\\";
			Str EntryPoint;
		};

		static bool CompileShader(ShaderDesc const & Input);
	};

	bool ShaderCompiler::CompileShader(ShaderDesc const & Input)
	{
		/*Core::Memory::UniquePtr<D3D_SHADER_MACRO[]> Macros = [](ShaderDesc const& MacrosInput) -> Core::Memory::UniquePtr<D3D_SHADER_MACRO[]>
		{
			Core::Memory::UniquePtr<D3D_SHADER_MACRO[]> Ret;

			if (MacrosInput.MacrosCount == 0)
			{
				Ret.Reset(new D3D_SHADER_MACRO[1]);
				Ret.Get()[0] = { nullptr, nullptr };

				return Ret;
			}

			Ret.Reset(new D3D_SHADER_MACRO[MacrosInput.MacrosCount]);
			for (size i = 0; i < MacrosInput.MacrosCount; ++i)
			{
				Ret[i] = { MacrosInput.Macros[i].Name.c_str(), MacrosInput.Macros[i].Value.c_str() };
			}

		}(Input);*/

		D3D_SHADER_MACRO Macros[] = { {nullptr, nullptr} };
		Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
		Microsoft::WRL::ComPtr <ID3DBlob> Errors;

		Str Desc = Input.ShaderAsset->GetType() == Asset::Shader::EShaderType::Pixel ? "ps_5_0" : "vs_5_0";

		HRESULT hr = D3DCompile(
			Input.ShaderAsset->GetSource(),
			Input.ShaderAsset->GetSourceLength(),
			Input.IncludePath.GetBuffer(),
			Macros,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			Input.EntryPoint.GetBuffer(),
			Desc.GetBuffer(),
			0,
			0,
			CodeBlob.GetAddressOf(),
			Errors.GetAddressOf()
		);

		if (SUCCEEDED(hr))
		{
			Input.ShaderAsset->BufferSizeInBytes = CodeBlob->GetBufferSize();
			Input.ShaderAsset->CompiledBuffer.Reset(new byte[Input.ShaderAsset->BufferSizeInBytes]);
			MemCpy(CodeBlob->GetBufferPointer(), Input.ShaderAsset->CompiledBuffer.Get(), Input.ShaderAsset->BufferSizeInBytes);

			return true;
		}

		const char* ErrorStr = (const char*)Errors->GetBufferPointer();
		LOG_ERROR(ErrorStr);
		BREAK();

		return false;
	}

}