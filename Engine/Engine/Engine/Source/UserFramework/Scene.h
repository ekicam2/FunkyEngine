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
	};

	struct IScene
	{
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick(f32 Delta) = 0;

		virtual i32 GetVisibleObjects(VisibleObject*& Objects) = 0;
	};

	struct Scene : public IScene
	{
		Scene() = default;

		VisibleObject Object;

		Core::Memory::UniquePtr<Asset::StaticMesh> MeshData;
		Core::Memory::UniquePtr<Asset::Material> MaterialData;

		virtual void Init() override;
		virtual void Shutdown() override {}
		virtual void Tick([[maybe_unused]] f32 Delta) override;
		virtual i32 GetVisibleObjects(VisibleObject*& Objects) override;
	};
}