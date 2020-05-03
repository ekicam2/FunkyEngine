#include "PostProcess.h"

#include "RenderingFrontend/ShaderCompiler.h"
#include "RenderingBackend/RenderingBackend.h"

#include "Core/Platform/Platform.h"

#include "Core/Assets/AssetRegistry.h"


Funky::Asset::ID Funky::Rendering::PostProcess::UnifiedVertexShader  = Asset::ID::Zero;
Funky::Asset::ID Funky::Rendering::PostProcess::PostProcessPlaneData = Asset::ID::Zero;


void Funky::Rendering::PostProcess::InitVS([[maybe_unused]]RenderingBackend* RB)
{
	//PostProcess::UnifiedVertexShader = AssetRegistry::GetInstance().CreateAsset<Asset::StaticMesh>("RealData/Shaders/Source/PPVS.hlsl");

	//ShaderCompiler::ShaderDesc Desc;
	//Desc.Api = Rendering::RenderingBackend::EAPI::DX11;
	//Desc.Source = Platform::ReadFile("RealData/Shaders/Source/PPVS.hlsl");
	//Desc.EntryPoint = "VSMain";
	//Desc.Type = Asset::Shader::EShaderType::Vertex;
	//
	//auto const & [Buffer, BufferSize] = ShaderCompiler::CompileShader(Desc);
	//RenderingBackend::ShaderInputDesc ShaderDesc;
	//ShaderDesc.ShaderData = Buffer;
	//ShaderDesc.DataSize = BufferSize;
	//
	//UnifiedVertexShader = RB->CreateVertexShader(&ShaderDesc);

}

Funky::Asset::ID Funky::Rendering::PostProcess::GetVSShader()
{
	return PostProcess::UnifiedVertexShader;
}

Funky::Rendering::PostProcess* Funky::Rendering::PostProcess::CreateFromSource([[maybe_unused]]Str FilePath, [[maybe_unused]] RenderingBackend* RB)
{
	PostProcess* Ret = new PostProcess();

	/*ShaderCompiler::ShaderDesc Desc;
	Desc.Api = Rendering::RenderingBackend::EAPI::DX11;
	Desc.Source = Platform::ReadFile(FilePath);
	Desc.EntryPoint = "PSMain";
	Desc.Type = Asset::Shader::EShaderType::Fragment;

	auto const& [Buffer, BufferSize] = ShaderCompiler::CompileShader(Desc);
	RenderingBackend::ShaderInputDesc ShaderDesc;
	ShaderDesc.ShaderData = Buffer;
	ShaderDesc.DataSize = BufferSize;

	Ret->PostProcessImplementation = RB->CreatePixelShader(&ShaderDesc);*/

	return Ret;
}

void Funky::Rendering::PostProcess::InitPostProcessSurface([[maybe_unused]] RenderingBackend* RB)
{
	//f32 PostProcessVertices[] = {
	//	-1.0f,  -1.0f, 0.0f, // dl
	//	-1.0f,  3.0f, 0.0f, // tl
	//	 3.0f, -1.0f, 0.0f // dr
	//};

	//PostProcessPlaneData = RB->CreateBuffer(
	//	3 * 3 * sizeof(f32),
	//	Rendering::RBuffer::EType::Vertex,
	//	Rendering::RBuffer::EUsageType::Static,
	//	&PostProcessVertices,
	//	sizeof(f32) * 3
	//);
}

Funky::Asset::ID Funky::Rendering::PostProcess::GetPostProcessSurface()
{
	return PostProcess::PostProcessPlaneData;
}

