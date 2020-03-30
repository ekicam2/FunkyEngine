#include "Material.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

Funky::Asset::Material* Funky::Asset::Material::CreateMaterialFromSourceCode(char const* VSSource, char const* PSSource)
{
	Material* Ret = new Material();
	Ret->VSSource = (VSSource);
	Ret->PSSource = (PSSource);
	return Ret;
}

//std::optional<std::pair<str, str>> Funky::Asset::Material::ParseMaterial(str const& Path)
//{
//	std::ifstream MaterialFile(Path, std::ios::binary);
//	if (!MaterialFile.is_open()) return {};
//
//	std::pair<str, str> Ret;
//
//	std::getline(MaterialFile, Ret.first);
//	std::getline(MaterialFile, Ret.second);
//
//	MaterialFile.close();
//
//	return Ret;
//}
