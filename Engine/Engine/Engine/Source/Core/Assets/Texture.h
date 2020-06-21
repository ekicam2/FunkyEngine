#pragma once

#include "Math/Vector2.h"
#include "Core/String.h"

namespace Funky::Asset
{
	struct Texture
	{
		friend class AssetRegistry;

		virtual ~Texture();
		Str Path;

		Math::Vec2u Size = Math::Vec2u::Zero;
		byte* Data = nullptr;

	private:
		static Texture* Create(Str const& Path);
	};

}