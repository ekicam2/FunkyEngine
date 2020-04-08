#include "Scene.h"

#include "FunkyEngine.h"
#include "Utils/MeshUtils.h"

void Funky::Scene::Init()
{
	LOG("Init Scene");

	bool bReverseIndices = true;
	auto [Vertrices, Indices] = MeshUtils::LoadOBJFromFile("RealData/Meshes/robot/robot.obj", bReverseIndices);
	MeshAsset.Reset(Asset::StaticMesh::CreateMeshFromRawData(Vertrices, Indices));
	Objects.Mesh = MeshAsset.Get();

	auto ShadersSource = Platform::ReadFile("RealData/Shaders/Source/ubershader.hlsl");
	auto ShaderVS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Vertex, ShadersSource);
	auto ShaderPS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Pixel, ShadersSource);

	MaterialAsset.Reset(Asset::Material::CreateMaterial(ShaderVS, ShaderPS));

	Objects.Material = MaterialAsset.Get();


	Funky::OnViewportResized.RegisterLambda([this](Math::Vec2u NewSize) { 
		Camera.MakePerspective((float)(NewSize.X) / NewSize.Y, 48.0f); 
	});

	Camera.MakePerspective((float)(2048u) / 1024u, 48.0f);

	Objects.Position += Math::Vec::FORWARD * 10.0f;
	Camera.Translate(Math::Vec::FORWARD * -10.0f);
}

void Funky::Scene::Tick(f32 Delta)
{
	//LOG("SceneTick: ", Delta);

	float Speed				= 0.025f * Delta;
	float RotationSpeed		= 0.2f * Delta;

	Speed *= FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::SHIFT) ? 0.25f : 1.0f;

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::W))
		Camera.Translate(Camera.GetForward() * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::S))
		Camera.Translate(-Camera.GetForward() * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::A))
		Camera.Translate(-Camera.GetRight() * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::D))
		Camera.Translate(Camera.GetRight() * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::E))
		Camera.Translate(Math::Vec::UP * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::Q))
		Camera.Translate(-Math::Vec::UP * Speed);

	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::SPACE))
		Objects.Rotation.Y += RotationSpeed;


	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::R))
	{
		auto ShadersSource = Platform::ReadFile("RealData/Shaders/Source/ubershader.hlsl");
		if (!ShadersSource.IsEmpty())
		{
			auto ShaderVS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Vertex, ShadersSource);
			auto ShaderPS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Pixel, ShadersSource);

			MaterialAsset.Reset(Asset::Material::CreateMaterial(ShaderVS, ShaderPS));

			Objects.Material = MaterialAsset.Get();
			Objects.Material = MaterialAsset.Get();
		}
	}

	 //Rotations 
	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_UP))
		Camera.Rotate(Math::Vec::RIGHT * Delta * 0.05f);
	
	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_DOWN))
		Camera.Rotate(-Math::Vec::RIGHT * Delta * 0.05f);
	
	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_LEFT))
		Camera.Rotate(Math::Vec::UP * RotationSpeed);
	
	if (FunkyEngine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_RIGHT))
		Camera.Rotate(-Math::Vec::UP * RotationSpeed);
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
