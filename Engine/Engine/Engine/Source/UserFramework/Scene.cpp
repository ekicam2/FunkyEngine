#include "Scene.h"
#include "Core/Timer.h"

#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Material.h"

#include "Engine.h"
#include "Utils/MeshUtils.h"
#include "RenderingFrontend\ShaderCompiler.h"


const static Str terrainMeshPath = "RealData/Meshes/Cube/gltf/Cube.gltf";
const static Str robotMeshPath = "RealData/Meshes/robot/robot.obj";
const static Str uberShaderPath = "RealData/Shaders/Source/ubershader.hlsl";

//#define MESH_PATH 

void Funky::Scene::Init()
{
	Asset::ID terrainMesh = Asset::ID::Zero;
	Asset::ID terrainMaterial = Asset::ID::Zero;
	
	auto& AR = Funky::AssetRegistry::GetInstance();
	Asset::ID sceneObjectMesh		= Asset::ID::Zero;
	Asset::ID sceneObjectMaterial	= Asset::ID::Zero;
	LOG("Load Scene Assets");
	{
		DEBUG_SCOPE_TIMER(TEXT("Loading assets"));

		terrainMesh			= AR.CreateAsset<Asset::StaticMesh>(terrainMeshPath);
		sceneObjectMesh		= AR.CreateAsset<Asset::StaticMesh>(robotMeshPath, true);

		sceneObjectMaterial = terrainMaterial = AR.CreateAsset<Asset::Material>(Asset::Material::ERenderingTechnique::Ubershader);
	}

	LOG("Init Scene");
	{
		DEBUG_SCOPE_TIMER(TEXT("Init scene"));

		InitTerrain({ 100, 100 }, terrainMesh, terrainMaterial);
		Terrain->Foreach([]([[maybe_unused]]size i, Tilemap::Tile& tile)
			{
				tile.Position += Math::Vec3f(0.0f, -1.0f, 0.0f);
			}
		);

		for (u8 i = 0; i < 9; ++i)
		{
			VisibleObject temp;
			temp.Mesh = sceneObjectMesh;
			temp.Material = sceneObjectMaterial;
			temp.Position = Math::Vec3f(rand() % Terrain->Size.X, 0.0f, rand() % Terrain->Size.Y);
			Objects.push_back(temp);
		}
	}


	Funky::OnViewportResized.RegisterLambda([this](Math::Vec2u NewSize) { 
		Camera.MakePerspective((float)(NewSize.X) / NewSize.Y, 48.0f); 
	});

	Camera.MakePerspective((float)(2048u) / 1024u, 48.0f);
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

	//if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::SPACE))
	//	Objects.Rotation.Y += RotationSpeed;


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

darray<Funky::VisibleObject> Funky::Scene::GetVisibleObjects()
{
	darray<Funky::VisibleObject> visibleObjects = Objects;

	const size terrainObjectsNum = Terrain->Data.size();
	for (size i = 0; i < terrainObjectsNum; ++i)
	{
		visibleObjects.push_back(Terrain->Data[i]);
	}


	return visibleObjects;
}

Math::Camera* Funky::Scene::GetCamera()
{
	return &Camera;
}

void Funky::Scene::InitTerrain(Math::Vec2u const& Size, Asset::ID mesh, Asset::ID material)
{
	Terrain.Reset(new Tilemap());
	Terrain->Size = Size;

	for (size i = 0; i < Size.X * Size.Y; ++i)
	{
		Tilemap::Tile newTile;
		const size width = Terrain->Size.X;
		const Math::Vec2u pos((u32)i % (u32)width, (u32)i / (u32)width);

		newTile.Material	= material;
		newTile.Mesh		= mesh;
		newTile.Position	= Math::Vec3f((f32)pos.X, 0.0f, (f32)pos.Y);

		Terrain->Data.push_back(newTile);
	}

	//Terrain->Foreach([=](size i, Tilemap::Tile& Tile)
	//	{
	//
	//	});
}