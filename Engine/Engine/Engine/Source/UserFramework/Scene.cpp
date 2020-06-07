#include "Scene.h"
#include "Core/Timer.h"

#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Material.h"

#include "Engine.h"
#include "Utils/MeshUtils.h"
#include "RenderingFrontend\ShaderCompiler.h"


const static Str terrainMeshPath	= "RealData/Meshes/Triangle/glTF/Triangle.gltf";//"RealData/Meshes/terrainplane.obj";
const static Str parentMeshPath		= "RealData/Meshes/Duck/glTF/Duck.gltf"; //"RealData/Meshes/Pirates/ship_dark.obj";
const static Str childMeshPath		= "RealData/Meshes/Avocado/glTF/Avocado.gltf"; //"RealData/Meshes/Pirates/ship_dark.obj";

const static Str uberShaderPath = "RealData/Shaders/Source/ubershader.hlsl";

//#define MESH_PATH 
Funky::Asset::ID parentMesh				= Funky::Asset::ID::Zero;
Funky::Asset::ID childMesh				= Funky::Asset::ID::Zero;
Funky::Asset::ID sceneObjectMaterial	= Funky::Asset::ID::Zero;

void Funky::Scene::Init()
{
	Asset::ID terrainMesh		= Asset::ID::Zero;
	Asset::ID terrainMaterial	= Asset::ID::Zero;
	
	auto& AR = Funky::AssetRegistry::GetInstance();
	LOG("Load Scene Assets");
	{
		DEBUG_SCOPE_TIMER(TEXT("Loading assets"));

		terrainMesh			= AR.CreateAsset<Asset::StaticMesh>(terrainMeshPath, true);
		parentMesh			= AR.CreateAsset<Asset::StaticMesh>(parentMeshPath, true);
		childMesh			= AR.CreateAsset<Asset::StaticMesh>(parentMeshPath, false);
		sceneObjectMaterial = terrainMaterial = AR.CreateAsset<Asset::Material>(Asset::Material::ERenderingTechnique::Ubershader);
	}

	LOG("Init Scene");
	{
		DEBUG_SCOPE_TIMER(TEXT("Init scene"));

		Math::Vec2u TerrainSize = { 50, 50 };
		InitTerrain(TerrainSize,
			[=]([[maybe_unused]] size i, Tilemap::Tile& tile)
			{
				tile.Material	= sceneObjectMaterial;
				tile.Mesh		= terrainMesh;
				tile.Scale		= 5.00f;
				tile.Rotation	= Math::Vec3f(-90.0f, 0.0f, 0.0f);

				const size width = TerrainSize.X;
				const Math::Vec2u pos((u32)i % (u32)width, (u32)i / (u32)width);
				tile.Position	= Math::Vec3f((f32)pos.X, 0.0f, (f32)pos.Y);
				tile.Position *= Math::Vec3f(5.0f, 1.0f, 5.0f);
				tile.Position += Math::Vec3f(0.0f, -1.0f, 0.0f);
			}
		);

		VisibleObject tempPlayer;
		tempPlayer.ChildrenCount = 1;
		tempPlayer.Rotation = Math::Vec3f(0.0f, 180.0f, 0.0f);
		tempPlayer.Scale = 1.0f;
		tempPlayer.Mesh = parentMesh;
		tempPlayer.Material = sceneObjectMaterial;
		Objects.push_back(tempPlayer);

		VisibleObject tempPlayerMesh;
		tempPlayerMesh.Position = Math::Vec3f(0.0f, -180.0f, 0.0f);
		tempPlayerMesh.Rotation = Math::Vec3f(0.0f, 90.0f, 0.0f);
		tempPlayerMesh.Scale = .25f;
		tempPlayerMesh.Mesh = childMesh;
		tempPlayerMesh.Material = sceneObjectMaterial;
		Objects.push_back(tempPlayerMesh);
	}


	Funky::OnViewportResized.RegisterLambda([this](Math::Vec2u NewSize) { 
		Camera.MakePerspective((float)(NewSize.X) / NewSize.Y, 48.0f, 1.0f, 1000.0f); 
	});

	Camera.MakePerspective((float)(2048u) / 1024u, 48.0f, 1.0f, 1000.0f);
	Camera.Translate(Objects[0].Position + Math::Vec3f(0.0f, 166 ,-200.0f));
	Camera.Rotate(Math::Vec3f(-20.0f, 0.0f, 0.0f));
}

void Funky::Scene::Tick(f32 Delta)
{
	//LOG("SceneTick: ", Delta);
	CaptureInput(Delta);
}

darray<Funky::VisibleObject> Funky::Scene::GetVisibleObjects()
{
	darray<Funky::VisibleObject> visibleObjects;

	//frustum culling

	for (size i = 0; i < Objects.size(); ++i)
		visibleObjects.push_back(Objects[i]);

	const size terrainObjectsNum = Terrain->Data.size();
	for (size i = 0; i < terrainObjectsNum; ++i)
		visibleObjects.push_back(Terrain->Data[i]);



	return visibleObjects;
}

Math::Camera* Funky::Scene::GetCamera()
{
	return &Camera;
}

void Funky::Scene::CaptureInput(f32 delta)
{
	auto const io = Engine::GetIO();
	Math::Camera*		camera			= &Camera;
		
	Math::Vec3f translation	 = Math::Vec3f::Zero;

	float movementSpeed = 0.025f;
	if (io->IsKeyPressed(Core::IO::EKey::SHIFT))
		movementSpeed *= 0.25f;

	const Math::Vec3f cameraForward	= camera->GetForward();
	if (io->IsKeyPressed(Core::IO::EKey::W))
		translation += (cameraForward * movementSpeed * delta);
	if (io->IsKeyPressed(Core::IO::EKey::S))
		translation += (-cameraForward * movementSpeed * delta);


	const Math::Vec3f cameraRight = camera->GetRight();
	if (io->IsKeyPressed(Core::IO::EKey::A))
		translation += (-cameraRight * movementSpeed * delta);
	if (io->IsKeyPressed(Core::IO::EKey::D))
		translation += (cameraRight * movementSpeed * delta);
		

	if (io->IsKeyPressed(Core::IO::EKey::E))
		translation += (Math::Vec::UP * movementSpeed * delta);
	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::Q))
		translation += (-Math::Vec::UP * movementSpeed * delta);

	if (translation != Math::Vec3f::Zero)
		camera->Translate(translation);


	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_UP))
	{
		auto moveDelta = Objects[0].GetForward() * delta * 0.02f;
		Objects[0].Position += moveDelta;
		//camera->Translate(moveDelta);
	}

	if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_DOWN))
	{
		auto moveDelta = -Objects[0].GetForward() * delta * 0.02f;
		Objects[0].Position += moveDelta;
		//camera->Translate(moveDelta);
	}

	// Mouse
	{
		

		Engine const* const engine = Engine::GetEngine();
		auto const hwnd = engine->GetHwnd();
		POINT cursorPosition = { 0, 0 };
		GetCursorPos(&cursorPosition);
		
		const float rotationSpeed = 0.2f;
		if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::RMB))
		{
			RECT WindowRect = { 0, 0, 0, 0 };
			GetWindowRect(hwnd, &WindowRect); //get window rect of control relative to screen
			POINT WindowCenter = {
				WindowRect.left + ((WindowRect.right - WindowRect.left) / 2),
				WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2)
			};
			SetCursorPos(WindowCenter.x, WindowCenter.y);

			POINT MouseOffset = {
				WindowCenter.x - cursorPosition.x,
				WindowCenter.y - cursorPosition.y
			};

			if (Engine::GetIO()->WasKeyPressed(Core::IO::EKey::RMB))
			{
				//SetCursorPos(WindowCenter.x, WindowCenter.y);
				Camera.RotateClamped(
					Math::Vec3f(
						MouseOffset.y * 0.05f,
						MouseOffset.x * rotationSpeed,
						0.0f
					),
					Math::Vec3f(1.0f, 0.0f, 0.0f),
					Math::Vec3f(89.0f, 0.0f, 0.0f)
				);
			}
		}
	
		//Rotations 

		if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_LEFT))
			Objects[0].Rotation += (-Math::Vec::UP * rotationSpeed * delta);

		if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::ARROW_RIGHT))
			Objects[0].Rotation += (Math::Vec::UP * rotationSpeed * delta);

		if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::SPACE))
		{
			const f32 distance = 200.0f;
			if (Engine::GetIO()->IsKeyPressed(Core::IO::EKey::TAB))
				camera->SetPosition(Objects[0].Position + (-Objects[0].GetForward()/*.RotateY(90.0f)*/ * distance) + Math::Vec3f(0.0f, 100.0f, 0.0));

			camera->LookAt(Objects[0].Position);
		}
	}
}
