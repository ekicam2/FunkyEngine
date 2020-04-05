#include "ITexture.h"

Funky::Asset::ITexture::ITexture([[maybe_unused]] Str const& Path, ETextureType Type) 
	: IAsset(Asset::EAssetType::Texture)
{
	Type = Type;
}
