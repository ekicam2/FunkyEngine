#include "ITexture.h"

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

Funky::Asset::ITexture::ITexture(str const& Path, TextureType Type) : IAsset(Path, Asset::EType::Texture)
{
	Type = Type;
}
