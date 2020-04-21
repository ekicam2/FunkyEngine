#include "Scene.h"

#include "Engine.h"
#include "Utils/MeshUtils.h"
#include "RenderingFrontend\ShaderCompiler.h"

void Funky::Scene::Init()
{
	LOG("Init Scene");

	auto& AR = Funky::AssetRegistry::GetInstance();

	Asset::StaticMesh::Desc MeshDesc;
	MeshDesc.bReverseIndices = true;
	MeshDesc.CreationType = Asset::StaticMesh::Desc::ECreationType::FromSource;
	MeshDesc.Path = "RealData/Meshes/robot/robot.obj";
	Objects.Mesh = AR.CreateAsset<Asset::StaticMesh>(MeshDesc);


	Asset::Shader::Desc ShaderDesc;
	ShaderDesc.Path = "RealData/Shaders/Source/ubershader.hlsl";

	ShaderDesc.Type = Asset::Shader::EShaderType::Vertex;
	auto VS = AR.CreateAsset<Asset::Shader>(ShaderDesc);

	ShaderDesc.Type = Asset::Shader::EShaderType::Fragment;
	auto PS = AR.CreateAsset<Asset::Shader>(ShaderDesc);
	
	ShaderCompiler::CompileShader(Funky::AssetRegistry::GetInstance().GetAsset<Asset::Shader>(VS));
	ShaderCompiler::CompileShader(Funky::AssetRegistry::GetInstance().GetAsset<Asset::Shader>(PS));
	

	Asset::Material::Desc MaterialDesc;
	MaterialDesc.Technique = Asset::Material::ERenderingTechnique::Toon;
	MaterialDesc.Name = "SmiesznyToon";
	MaterialDesc.VS = VS;
	MaterialDesc.PS = PS;

	Objects.Material = AR.CreateAsset<Asset::Material>(MaterialDesc);


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

	Speed *= Engine::GetIO()->IsKeyPressed(Core::IO::EKey::SHIFT) ? 0.25f : 1.0f;

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::W))
		Camera.Translate(Camera.GetForward() * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::S))
		Camera.Translate(-Camera.GetForward() * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::A))
		Camera.Translate(-Camera.GetRight() * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::D))
		Camera.Translate(Camera.GetRight() * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::E))
		Camera.Translate(Math::Vec::UP * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::Q))
		Camera.Translate(-Math::Vec::UP * Speed);

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::SPACE))
		Objects.Rotation.Y += RotationSpeed;


	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::R))
	{
		auto ShadersSource = Platform::ReadFile("RealData/Shaders/Source/ubershader.hlsl");
		if (!ShadersSource.IsEmpty())
		{
			//auto ShaderVS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Vertex, ShadersSource);
			//auto ShaderPS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Fragment, ShadersSource);

			//MaterialAsset.Reset(Asset::Material::CreateMaterial(ShaderVS, ShaderPS));

			//Objects.Material = MaterialAsset.Get();
			//Objects.Material = MaterialAsset.Get();
		}
	}

	 //Rotations 
	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_UP))
		Camera.Rotate(Math::Vec::RIGHT * Delta * 0.05f);
	
	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_DOWN))
		Camera.Rotate(-Math::Vec::RIGHT * Delta * 0.05f);
	
	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_LEFT))
		Camera.Rotate(Math::Vec::UP * RotationSpeed);
	
	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_RIGHT))
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
