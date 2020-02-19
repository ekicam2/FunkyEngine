#pragma once

#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Material.h"

namespace Funky
{
	struct VisibleObject
	{
		Asset::StaticMesh* Mesh;
		Asset::Material* Material;
		Math::Vec3f Position = Math::Vec3f(0.0f, 0.0f, 0.0f);
	};

	struct Scene
	{
		Scene() = default;

		VisibleObject Objects;

		void Init() {}
		void Shutdown() {}
		void Tick([[maybe_unused]] f32 Delta) {}
	};
}