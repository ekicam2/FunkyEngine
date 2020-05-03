#include "Material.h"

#include "Core/Assets/AssetRegistry.h"
#include "Core/Assets/Shader.h"

Funky::Asset::Material* Funky::Asset::Material::CreateFromDesc(Desc const& desc)
{
	auto ret = new Funky::Asset::Material();
	
	ret->PS = desc.PS;
	ret->VS = desc.VS;
	ret->Technique = desc.Technique;

	return ret;
}

Funky::Asset::Material* Funky::Asset::Material::Create(ERenderingTechnique technique)
{
	return Create(Material::MaterialTechniqueToPath[static_cast<u8>(technique)], technique);
}

Funky::Asset::Material* Funky::Asset::Material::Create(Str const& path, ERenderingTechnique technique /*= ERenderingTechnique::Custom*/)
{
	return Create(path, path, technique);
}

Funky::Asset::Material* Funky::Asset::Material::Create(Str const& path_vs, Str const& path_ps, ERenderingTechnique technique /*= ERenderingTechnique::Custom*/)
{
	auto ret = new Material();
	ret->Path = path_ps;
	ret->Technique = technique;

	auto& ar = Funky::AssetRegistry::GetInstance();
	ret->VS = ar.CreateAsset<Asset::Shader>(path_vs, Asset::Shader::EShaderType::Vertex);
	ret->PS = ar.CreateAsset<Asset::Shader>(path_ps, Asset::Shader::EShaderType::Fragment);

	return ret;
}
