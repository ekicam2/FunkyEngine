#include "PostProcess.h"

#include "Core/Platform/Platform.h"

Funky::Rendering::RBuffer* Funky::Rendering::PostProcess::PostProcessPlaneData = nullptr;
Funky::Rendering::RShader* Funky::Rendering::PostProcess::UnifiedVertexShader = nullptr;

void Funky::Rendering::PostProcess::InitVS()
{
	auto Source = Platform::ReadFile("RealData/Shaders/Source/PPVS.hlsl");

}

Funky::Rendering::RShader* Funky::Rendering::PostProcess::GetVSShader()
{
	return UnifiedVertexShader;
}

Funky::Rendering::PostProcess* Funky::Rendering::PostProcess::CreateFromSource(Str FilePath)
{
	return nullptr;
}

void Funky::Rendering::PostProcess::InitPostProcessSurface(RenderingBackend* RB)
{
	f32 PostProcessVertices[] = {
		-1.0f,  -1.0f, 0.0f, // dl
		-1.0f,  3.0f, 0.0f, // tl
		 3.0f, -1.0f, 0.0f // dr
	};

	PostProcessPlaneData = RB->CreateBuffer(
		3 * 3 * sizeof(f32),
		Rendering::RBuffer::EType::Vertex,
		Rendering::RBuffer::EUsageType::Static,
		&PostProcessVertices,
		sizeof(f32) * 3
	);
}

Funky::Rendering::RBuffer* Funky::Rendering::PostProcess::GetPostProcessSurface()
{
	return PostProcessPlaneData;
}

