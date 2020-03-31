#include "Material.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

#include "Core/Memory/Memory.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include <wrl/client.h>

Funky::Asset::Material* Funky::Asset::Material::CreateMaterialFromSourceCode(char const* VSSource, char const* PSSource)
{
	Material* Ret = new Material();
	Ret->VSSource = (VSSource);
	Ret->PSSource = (PSSource);
	return Ret;
}

void Funky::Asset::Material::Compile(Rendering::RenderingBackend::API BackendApi)
{
	bIsCompiled = false;

	if (BackendApi == Rendering::RenderingBackend::API::DX11)
	{
		{
			D3D_SHADER_MACRO Macros[] = { nullptr };
			Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
			Microsoft::WRL::ComPtr <ID3DBlob> Errors;

			HRESULT hr = D3DCompile(
				GetVertexShaderSourceCode(),
				GetVertexShaderSourceCodeLength(),
				GetPath().c_str(),
				Macros,
				NULL,
				"VSMain",
				"vs_5_0",
				0,
				0,
				CodeBlob.GetAddressOf(),
				Errors.GetAddressOf()
			);

			if (SUCCEEDED(hr))
			{
				CompiledVSSize = CodeBlob->GetBufferSize();
				CompiledVS.Reset(new byte[CompiledVSSize]);
				Memcpy(CodeBlob->GetBufferPointer(), CompiledVS.Get(), CompiledVSSize);
			}
			else
			{
				const char* ErrorStr = (const char*)Errors->GetBufferPointer();
				LOG_ERROR(ErrorStr);

				CHECK(SUCCEEDED(hr));
				return;
			}
		}

		/////////////////////////
		{
			D3D_SHADER_MACRO Macros[] = { nullptr };
			Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
			Microsoft::WRL::ComPtr <ID3DBlob> Errors;

			HRESULT hr = D3DCompile(
				GetPixelShaderSourceCode(),
				GetPixelShaderSourceCodeLength(),
				GetPath().c_str(),
				Macros,
				NULL,
				"PSMain",
				"ps_5_0",
				0,
				0,
				CodeBlob.GetAddressOf(),
				Errors.GetAddressOf()
			);

			if (SUCCEEDED(hr))
			{
				CompiledPSSize = CodeBlob->GetBufferSize();
				CompiledPS.Reset(new byte[CompiledPSSize]);
				Memcpy(CodeBlob->GetBufferPointer(), CompiledPS.Get(), CompiledPSSize);
			}
			else
			{
				const char* ErrorStr = (const char*)Errors->GetBufferPointer();
				LOG_ERROR(ErrorStr);

				CHECK(SUCCEEDED(hr));
				return;
			}
		}

		bIsCompiled = true;
	}
}

//std::optional<std::pair<str, str>> Funky::Asset::Material::ParseMaterial(str const& Path)
//{
//	std::ifstream MaterialFile(Path, std::ios::binary);
//	if (!MaterialFile.is_open()) return {};
//
//	std::pair<str, str> Ret;
//
//	std::getline(MaterialFile, Ret.first);
//	std::getline(MaterialFile, Ret.second);
//
//	MaterialFile.close();
//
//	return Ret;
//}
