#include "Scene.h"



void Funky::Scene::Init()
{
	LOG("Init Scene");

	darray<Vertex> vertices ({
		{
			{ 0.0f, 1.0f, 0.0f }, // pos  1
			{ 0.0f, 1.0f, 0.0f }, // col  1
			{ 0.0f, 0.0f, 0.0f }, // norm 1
			{ 0.5f, 1.0f }		  // uv	  1
		},
		{
			{ 1.0f, 0.0f, 0.0f }, // pos  2
			{ 1.0f, 0.0f, 0.0f }, // col  2
			{ 0.0f, 0.0f, 0.0f }, // norm 2
			{ 0.5f, 1.0f }		  // uv   2
		},
		{
			{ -1.0f, 0.0f, 0.0f }, // pos  3
			{ -1.0f, 0.0f, 0.0f }, // col  3
			{  0.0f, 0.0f, 0.0f }, // norm 3
			{  0.5f, 1.0f }		   // uv   3
		}
	});

	darray<u16> indices({ 0, 1, 2 });

	MeshData.Reset(Asset::StaticMesh::CreateMeshFromRawData(vertices, indices));
	Object.Mesh = MeshData.Get();

	const char* vs = R"(
			cbuffer ModelViewProjectionConstantBuffer : register(b0)
			{
				// REMEMBER IN HLSL matrices are column major
				matrix mvp;
				matrix model;
				float3 lookat;
				float padding;
			};

			float4 VSMain(float3 vPosition : POSITION) : SV_POSITION
			{	
				matrix cosglupiego = model * mvp;
				float4 Pos = mul(mvp, float4(vPosition, 1.0));
				return Pos;//float4(vPosition, 1.0);
			}
		)";

	const char* ps = R"(
			float4 PSMain(float4 vPosition : SV_POSITION) : SV_TARGET0 
			{	
				return float4(1.0f, 0.0f, 1.0f, 1.0f);
			}
		)";
	MaterialData.Reset(Asset::Material::CreateMaterialFromSourceCode(vs, ps));
	Object.Material = MaterialData.Get();

	Object.Position += Math::Vec::FORWARD * 2.0f;
}

void Funky::Scene::Tick([[maybe_unused]] f32 Delta)
{
	//LOG("SceneTick: ", Delta);
}

i32 Funky::Scene::GetVisibleObjects(VisibleObject*& Objects)
{
	Objects = &Object;
	return 1;
}
