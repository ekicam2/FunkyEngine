#pragma once

#include "Core/Assets/ITexture.h"

namespace Funky::Asset
{
	struct Texture : ITexture
	{
		static Texture* CreateFromFile(str const & pFilePath);
		virtual byte* GetData() const override;

	private:
		SDL_Surface* Image;
		darray<u32> Data;

		Texture(str const& Path, TextureType Type);
		~Texture();
	};
}


