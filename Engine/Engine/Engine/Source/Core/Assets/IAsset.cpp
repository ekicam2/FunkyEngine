#include "IAsset.h"

str const Funky::Asset::TypeToString(Asset::Type Type)
{
	switch (Type)
	{
	case Asset::Type::Texture:
		return "Texture";
	case Asset::Type::Mesh:
		return "Mesh";
	case Asset::Type::Material:
		return "Material";
	default:
		return "Unknown";
	}
}

Funky::Asset::Type const Funky::Asset::StringToType(str const& StrType)
{
	if (StrType == "Texture")
		return Asset::Type::Texture;
	else if (StrType == "Mesh")
		return Asset::Type::Mesh;
	else if (StrType == "Material")
		return Asset::Type::Material;

	return Asset::Type::Unknown;
}
