#include "ITexture.h"


Funky::Asset::ITexture::ITexture(str const& Path, TextureType Type) : IAsset(Path, Asset::EType::Texture)
{
	Type = Type;
}
