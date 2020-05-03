#pragma once

#include "RenderingBackend/RenderingBackend.h"
#include "Core/Assets/Material.h"
#include "Core/Assets/Shader.h"
#include "Core/Memory/Memory.h"

#include "DebugMacros.h"
#include "LogMacros.h"

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
		static void CompileShader(Asset::Shader* Input);

	private:
		struct ShaderDesc
		{

			Rendering::RenderingBackend::EAPI Api;
			Asset::Shader::EShaderType Type;
			Str Name = "Unknown";
			Str Source;

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

		struct CompiledData
		{
			Core::Memory::UniquePtr<byte[]> Buffer;
			size BufferSize;

		};

		static CompiledData CompileShader(ShaderDesc const& Input);
	};

	inline void ShaderCompiler::CompileShader(Asset::Shader* Input)
	{
		ShaderCompiler::ShaderDesc Desc;
		Desc.Api = Rendering::RenderingBackend::EAPI::DX11;
		Desc.Source = Input->GetSource();
		Desc.EntryPoint = Input->GetType() == Asset::Shader::EShaderType::Fragment ? "PSMain" : "VSMain";
		Desc.Type = Input->GetType();

		auto [CompiledBuffer, BufferSize] = CompileShader(Desc);

		Input->BufferSizeInBytes = BufferSize;
		Input->CompiledBuffer.Reset(new byte[Input->BufferSizeInBytes]);
		MemCpy(CompiledBuffer, Input->CompiledBuffer.Get(), Input->BufferSizeInBytes);
		Input->bIsCompiled = true;
	}

	inline ShaderCompiler::CompiledData ShaderCompiler::CompileShader(ShaderDesc const & Input)
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

		const Str Desc = Input.Type == Asset::Shader::EShaderType::Fragment ? "ps_5_0" : "vs_5_0";

		HRESULT hr = D3DCompile(
			Input.Source.GetBuffer(),
			Input.Source.Length(),
			(Input.IncludePath + Input.Name).GetBuffer(),
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
			ShaderCompiler::CompiledData Output;

			Output.BufferSize = CodeBlob->GetBufferSize();
			Output.Buffer.Reset(new byte[Output.BufferSize]);
			MemCpy(CodeBlob->GetBufferPointer(), Output.Buffer.Get(), Output.BufferSize);

			return Output;
		}

		const char* ErrorStr = (const char*)Errors->GetBufferPointer();
		LOG_ERROR(ErrorStr);
		BREAK();

		return {};
	}

}