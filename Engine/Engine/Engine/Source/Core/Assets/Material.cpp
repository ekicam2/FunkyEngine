#include "Material.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

#include "Core/Memory/Memory.h"



Funky::Asset::Material* Funky::Asset::Material::CreateMaterial(Funky::Asset::Shader* VS, Funky::Asset::Shader* PS)
{
	CHECK(VS != nullptr && VS->GetType() == Shader::EShaderType::Vertex);
	CHECK(PS != nullptr && PS->GetType() == Shader::EShaderType::Pixel);

	Material* Ret = new Material();
	Ret->VS = VS;
	Ret->PS = PS;
	return Ret;
}

Funky::Asset::Shader* Funky::Asset::Shader::CreateFromFile(EShaderType InType, Str const& InPath)
{
	auto Ret = new Shader();
	Ret->Type = InType;
	Ret->Source = Move(Platform::ReadFile(InPath));

	return Ret;
}

Funky::Asset::Shader* Funky::Asset::Shader::CreateShaderFromSource(EShaderType InType, Str const& InSource)
{
	auto Ret = new Shader();
	Ret->Type = InType;
	Ret->Source = InSource;

	return Ret;
}
