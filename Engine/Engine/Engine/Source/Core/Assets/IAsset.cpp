#include "IAsset.h"

Str const Funky::Asset::TypeToString(Asset::EAssetType Type)
{
	switch (Type)
	{
	case Asset::EAssetType::Texture:
		return "Texture";
	case Asset::EAssetType::Mesh:
		return "Mesh";
	case Asset::EAssetType::Material:
		return "Material";
	default:
		return "Unknown";
	}
}

Funky::Asset::EAssetType const Funky::Asset::StringToType(Str const& StrType)
{
	if (StrType == "Texture")
		return Asset::EAssetType::Texture;
	else if (StrType == "Mesh")
		return Asset::EAssetType::Mesh;
	else if (StrType == "Material")
		return Asset::EAssetType::Material;

	return Asset::EAssetType::Unknown;
}
