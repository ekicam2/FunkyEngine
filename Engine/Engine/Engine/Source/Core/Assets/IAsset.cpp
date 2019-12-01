#include "IAsset.h"

str const Funky::Asset::TypeToString(Asset::EType Type)
{
	switch (Type)
	{
	case Asset::EType::Texture:
		return "Texture";
	case Asset::EType::Mesh:
		return "Mesh";
	case Asset::EType::Material:
		return "Material";
	default:
		return "Unknown";
	}
}

Funky::Asset::EType const Funky::Asset::StringToType(str const& StrType)
{
	if (StrType == "Texture")
		return Asset::EType::Texture;
	else if (StrType == "Mesh")
		return Asset::EType::Mesh;
	else if (StrType == "Material")
		return Asset::EType::Material;

	return Asset::EType::Unknown;
}
