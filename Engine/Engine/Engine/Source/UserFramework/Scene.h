#pragma once

#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Material.h"

#include "Core/Memory/UniquePtr.h"

namespace Funky
{
	struct VisibleObject
	{
		Asset::ID Mesh;
		Asset::ID Material;

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

	struct Tilemap
	{
		struct Tile
		{
			Asset::ID Mesh;
			Asset::ID Material;

			Math::Vec2u Position;
		};

		Math::Vec2u Size = Math::Vec2u::Zero;
		Math::Vec2u Position;


		template <typename LAMBDA>
		void Foreach(LAMBDA&& F)
		{
			const auto width = Size.x;
			for (size i = 0; i < width; ++i)
			{
				auto [x_ws, y_ws] = ()[i, Size] -> Math::Vec2u {
					return { i % 4, i / 4 };
				}();

				CHECK(Data[i].Position == Math::Vec2u(x_ws, y_ws));
				F(x_ws, y_ws);
			}
		}



		darray<Tile> Data;
	};

	struct Scene : public IScene
	{
		Scene() = default;

		Math::Camera Camera;
		VisibleObject Objects;

		virtual void Init() override;
		virtual void Shutdown() override {}
		virtual void Tick([[maybe_unused]] f32 Delta) override;
		virtual i32 GetVisibleObjects(VisibleObject*& Objects) override;
		virtual Math::Camera* GetCamera() override;

	};
}