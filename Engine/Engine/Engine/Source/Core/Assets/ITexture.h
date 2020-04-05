#pragma once

#include "Math/Vector2.h"
#include "Core/Assets/IAsset.h"

namespace Funky
{
	namespace Asset
	{
		struct ITexture : public IAsset
		{
			enum class ETextureType
			{
				UNKNOWN, // !< Not supported
				Tex_1D,
				Tex_2D,
				Tex_6D
			} Type;

			ITexture(Str const& Path, ETextureType Type);

			Math::Vec2u Size = Math::Vec2u(0u, 0u);
		
			virtual byte* GetData() const = 0;
		};

	}
}