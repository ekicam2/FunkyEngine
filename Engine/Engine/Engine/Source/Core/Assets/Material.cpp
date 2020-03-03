#include "Material.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

std::optional<std::pair<str, str>> Funky::Asset::Material::ParseMaterial(str const& Path)
{
	std::ifstream MaterialFile(Path, std::ios::binary);
	if (!MaterialFile.is_open()) return {};

	std::pair<str, str> Ret;

	std::getline(MaterialFile, Ret.first);
	std::getline(MaterialFile, Ret.second);

	MaterialFile.close();

	return Ret;
}
