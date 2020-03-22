#include "Scene.h"

#include "FunkyEngine.h"

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

	MeshAsset.Reset(Asset::StaticMesh::CreateMeshFromRawData(vertices, indices));
	Objects.Mesh = MeshAsset.Get();

	const char* vs = R"(
			cbuffer ModelViewProjectionConstantBuffer : register(b0)
			{
				// REMEMBER IN HLSL matrices are column major
				matrix mvp;
				matrix model;
				float3 lookat;
				float padding;
			};

			float4 VSMain(float3 vPosition : POSITION, float3 vColor : COLOR, float3 vNormal : NORMAL, float2 vUv : UV) : SV_POSITION
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

	Camera.MakePerepective((float)(2048u) / 1024u);

	MaterialAsset.Reset(Asset::Material::CreateMaterialFromSourceCode(vs, ps));
	Objects.Material = MaterialAsset.Get();

	Objects.Position += Math::Vec::FORWARD * 10.0f;
}

void Funky::Scene::Tick([[maybe_unused]] f32 Delta)
{
	//LOG("SceneTick: ", Delta);

	float Speed = 0.025f*Delta;
	Speed *= FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::SHIFT) ? 0.25f : 1.0f;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::W))
		Objects.Position += Math::Vec::FORWARD * Speed;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::S))
		Objects.Position += -Math::Vec::FORWARD * Speed;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::A))
		Objects.Position += -Math::Vec::RIGHT * Speed;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::D))
		Objects.Position += Math::Vec::RIGHT * Speed;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::E))
		Objects.Position += Math::Vec::UP * Speed;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::Q))
		Objects.Position += -Math::Vec::UP * Speed;
}

i32 Funky::Scene::GetVisibleObjects(VisibleObject*& InOutObjects)
{
	InOutObjects = &Objects;
	return 1;
}

Math::Camera* Funky::Scene::GetCamera()
{
	return &Camera;
}
