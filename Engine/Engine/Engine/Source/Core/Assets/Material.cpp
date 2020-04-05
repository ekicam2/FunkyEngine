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

Funky::Asset::Shader* Funky::Asset::Shader::CreateShaderFromSource(EShaderType InType, Str const& InSource)
{
	auto Ret = new Shader();
	Ret->Type = InType;

	Ret->SourceLength = InSource.Length();
	Ret->Source.Reset(new char[Ret->SourceLength]);
	MemCpy(InSource.GetBuffer(), Ret->Source.Get(), Ret->SourceLength);

	return Ret;
}
