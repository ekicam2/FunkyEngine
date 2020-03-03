#include "ITexture.h"

Funky::Asset::ITexture::ITexture([[maybe_unused]]str const& Path, TextureType Type) : IAsset(Asset::EType::Texture)
{
	Type = Type;
}
