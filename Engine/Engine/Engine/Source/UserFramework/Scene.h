#pragma once

#include "Math/Math.h"
#include "Core/Containers.h"
#include "Core/Memory/UniquePtr.h"

#include "Core/Assets/AssetRegistry.h"

namespace Funky
{
	struct VisibleObject
	{
		struct BoundingShpere
		{
			Math::Vec3f Position	= Math::Vec3f(0.0f, 0.0f, 0.0f);
			f32			Radius		= 0.0f;
			f32			Scale		= 0.0f;
		};

		Asset::ID Mesh = Asset::ID::Zero;
		Asset::ID Material = Asset::ID::Zero;
		BoundingShpere BoundingSphere() const
		{
			auto& ar = AssetRegistry::GetInstance();
			Asset::StaticMesh const * mesh = ar.GetAsset<Asset::StaticMesh>(Mesh);
			auto const aabb = mesh->GetAABB();
			const f32 radius = Math::Max(aabb.Min, aabb.Max).Length();

			return { Position, radius, Scale };
		}

		Math::Vec3f GetForward() const
		{
			Math::Vec3f ret(0.0f, 0.0f, -1.f);

			ret = ret.RotateX(Rotation.X);
			ret = ret.RotateY(-Rotation.Y);
			ret = ret.RotateZ(Rotation.Z);

			return ret.Normalized();
		}

		Math::Vec3f Position = Math::Vec3f(0.0f, 0.0f, 0.0f);
		Math::Vec3f Rotation = Math::Vec3f(0.0f, 0.0f, 0.0f);
		f32 Scale = 1.0f;
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

	struct Tilemap
	{
		using  Tile = VisibleObject;

		Math::Vec2u Size = Math::Vec2u::Zero;
		Math::Vec3f Origin = Math::Vec3f::Zero;

		Tile& GetAt(Math::Vec2u const& position)
		{
			const size index = position.X * Size.Y + position.X;
			CHECK(index < Data.size());
			return Data[index];
		}

		template <typename LAMBDA>
		void Foreach(LAMBDA&& F)
		{
			const auto width = Data.size();
			for (size i = 0; i < width; ++i)
			{
				F(i, Data[i]);
			}
		}

		darray<Tile> Data;
	};

	struct Scene : public IScene
	{
		Scene() = default;

		Math::Camera Camera;
		darray<VisibleObject> Objects;

		virtual void Init() override;
		virtual void Shutdown() override {}
		virtual void Tick([[maybe_unused]] f32 Delta) override;
		virtual darray<VisibleObject> GetVisibleObjects() override;
		virtual Math::Camera* GetCamera() override;

		Core::Memory::UniquePtr<Tilemap> Terrain;
		void InitTerrain(Math::Vec2u const& Size, Asset::ID mesh, Asset::ID material);

		void CaptureInput(f32 delta);
	};
}