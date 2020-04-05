#pragma once

#include "Core/Assets/ITexture.h"
#include "Core/Containers.h"

namespace Funky::Asset
{
	struct Texture : ITexture
	{
		static Texture* CreateFromFile(Str const & pFilePath);
		virtual byte* GetData() const override;

	private:
		darray<u32> Data;

		Texture(Str const& Path, ETextureType Type);
		~Texture();
	};
}


