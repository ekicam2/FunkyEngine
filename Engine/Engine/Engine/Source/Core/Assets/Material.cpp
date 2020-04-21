#include "Material.h"

Funky::Asset::Material* Funky::Asset::Material::CreateFromDesc(Desc const& desc)
{
	auto ret = new Funky::Asset::Material();
	
	ret->PS = desc.PS;
	ret->VS = desc.VS;
	ret->Technique = desc.Technique;

	return ret;
}
