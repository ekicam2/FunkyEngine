#include "ITexture.h"


Funky::Asset::ITexture::ITexture(str const& Path, TextureType Type) : IAsset(Path, Asset::Type::Texture)
{
	Type = Type;
}
