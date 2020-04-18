#pragma once

#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Material.h"

#include "Core/Memory/UniquePtr.h"

namespace Funky
{
	struct VisibleObject
	{
		Asset::StaticMesh* Mesh;
		Asset::Material* Material;

		Math::Vec3f Position = Math::Vec3f(0.0f, 0.0f, 0.0f);
		Math::Vec3f Rotation = Math::Vec3f(0.0f, 0.0f, 0.0f);

	};

	struct IScene
	{
		virtual ~IScene() {}
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick(f32 Delta) = 0;

		virtual i32 GetVisibleObjects(VisibleObject*& Objects) = 0;
		virtual Math::Camera* GetCamera() = 0;

	};

	struct Scene : public IScene
	{
		Scene() = default;

		Math::Camera Camera;
		VisibleObject Objects;

		Core::Memory::UniquePtr<Asset::StaticMesh> MeshAsset;
		Core::Memory::UniquePtr<Asset::Material> MaterialAsset;

		virtual void Init() override;
		virtual void Shutdown() override {}
		virtual void Tick([[maybe_unused]] f32 Delta) override;
		virtual i32 GetVisibleObjects(VisibleObject*& Objects) override;
		virtual Math::Camera* GetCamera() override;
	};
}