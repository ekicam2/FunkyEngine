#include "Shader.h"

#include "Core/Platform/Platform.h"

Funky::Asset::Shader* Funky::Asset::Shader::CreateFromDesc(Desc const& desc)
{
	Funky::Asset::Shader* ret = new Funky::Asset::Shader();
	ret->Type = desc.Type;

	if (!desc.Source.IsEmpty())
	{
		ret->Source = desc.Source;
	}
	else
	{
		ret->Source = Platform::ReadFile(desc.Path);
	}

	return ret;
}
