#include "Material.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

#include "Core/Memory/Memory.h"



Funky::Asset::Material* Funky::Asset::Material::CreateMaterial(Funky::Asset::Shader* VS, Funky::Asset::Shader* PS)
{
	CHECK(VS != nullptr && VS->GetType() == Shader::EShaderType::Vertex);
	CHECK(PS != nullptr && PS->GetType() == Shader::EShaderType::Pixel);

	Material* Ret = new Material();
	Ret->VS = VS;
	Ret->PS = PS;
	return Ret;
}


//void Funky::Asset::Material::Compile(Rendering::RenderingBackend::EAPI BackendApi)
//{
//	bIsValid = false;
//
//	if (BackendApi == Rendering::RenderingBackend::EAPI::DX11)
//	{
//		{
//			D3D_SHADER_MACRO Macros[] = { nullptr };
//			Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
//			Microsoft::WRL::ComPtr <ID3DBlob> Errors;
//
//			HRESULT hr = D3DCompile(
//				GetVertexShaderSourceCode(),
//				GetVertexShaderSourceCodeLength(),
//				GetPath().c_str(),
//				Macros,
//				NULL,
//				"VSMain",
//				"vs_5_0",
//				0,
//				0,
//				CodeBlob.GetAddressOf(),
//				Errors.GetAddressOf()
//			);
//
//			if (SUCCEEDED(hr))
//			{
//				CompiledVSSize = CodeBlob->GetBufferSize();
//				CompiledVS.Reset(new byte[CompiledVSSize]);
//				MemCpy(CodeBlob->GetBufferPointer(), CompiledVS.Get(), CompiledVSSize);
//			}
//			else
//			{
//				const char* ErrorStr = (const char*)Errors->GetBufferPointer();
//				LOG_ERROR(ErrorStr);
//
//				CHECK(SUCCEEDED(hr));
//				return;
//			}
//		}
//
//		/////////////////////////
//		{
//			D3D_SHADER_MACRO Macros[] = { nullptr };
//			Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
//			Microsoft::WRL::ComPtr <ID3DBlob> Errors;
//
//			HRESULT hr = D3DCompile(
//				GetPixelShaderSourceCode(),
//				GetPixelShaderSourceCodeLength(),
//				GetPath().c_str(),
//				Macros,
//				NULL,
//				"PSMain",
//				"ps_5_0",
//				0,
//				0,
//				CodeBlob.GetAddressOf(),
//				Errors.GetAddressOf()
//			);
//
//			if (SUCCEEDED(hr))
//			{
//				CompiledPSSize = CodeBlob->GetBufferSize();
//				CompiledPS.Reset(new byte[CompiledPSSize]);
//				MemCpy(CodeBlob->GetBufferPointer(), CompiledPS.Get(), CompiledPSSize);
//			}
//			else
//			{
//				const char* ErrorStr = (const char*)Errors->GetBufferPointer();
//				LOG_ERROR(ErrorStr);
//
//				CHECK(SUCCEEDED(hr));
//				return;
//			}
//		}
//
//		bIsValid = true;
//	}
//}

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

Funky::Asset::Shader* Funky::Asset::Shader::CreateShaderFromSource(EShaderType InType, str const& InSource)
{
	auto Ret = new Shader();
	Ret->Type = InType;

	Ret->SourceLength = InSource.size();
	Ret->Source.Reset(new char[Ret->SourceLength]);
	MemCpy(InSource.data(), Ret->Source.Get(), Ret->SourceLength);

	return Ret;
}
