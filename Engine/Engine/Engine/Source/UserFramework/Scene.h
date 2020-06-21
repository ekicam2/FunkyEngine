#pragma once

#include "Math/Math.h"
#include "Core/Containers.h"
#include "Core/Memory/UniquePtr.h"

#include "Core/Assets/AssetRegistry.h"

namespace Funky
{
	struct Transform
	{
		Math::Vec3f Position	= Math::Vec3f::Zero;
		Math::Vec3f Rotation	= Math::Vec3f::Zero;
		Math::Vec3f Scale		= { 1.0f, 1.0f, 1.0f };

		Math::Vec3f GetForward() const
		{
			Math::Vec3f ret(0.0f, 0.0f, -1.f);

			ret = ret.RotateX(Rotation.X);
			ret = ret.RotateY(-Rotation.Y);
			ret = ret.RotateZ(Rotation.Z);

			return ret.Normalized();
		}
	};

	struct BoundingShpere
	{
		Math::Vec3f Position	= Math::Vec3f(0.0f, 0.0f, 0.0f);
		f32			Radius		= 0.0f;
		f32			Scale		= 0.0f;
	};

	struct VisibleObject
	{
		size ChildrenCount = 0;

		Asset::ID Mesh		= Asset::ID::Zero;
		Asset::ID Material	= Asset::ID::Zero;

		BoundingShpere BoundingSphere() const
		{
			auto& ar = AssetRegistry::GetInstance();
			Asset::StaticMesh const * mesh = ar.GetAsset<Asset::StaticMesh>(Mesh);
			auto const aabb = mesh->GetAABB();
			const f32 radius = Math::Max(aabb.Min, aabb.Max).Length();

			return { Transform.Position, radius, Transform.Scale.X };
		}

		Transform Transform;
	};

	struct IScene
	{
		virtual ~IScene() {}
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick(f32 Delta) = 0;

		virtual darray<VisibleObject> GetVisibleObjects() = 0;
		virtual Math::Camera* GetCamera() = 0;

	};


}