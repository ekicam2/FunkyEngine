#include "Shader.h"

#include "Core/Platform/Platform.h"

#include "RenderingFrontend\ShaderCompiler.h"

Funky::Asset::Shader* Funky::Asset::Shader::Create(Str const& path, EShaderType type)
{
	Funky::Asset::Shader* ret = new Funky::Asset::Shader();
	ret->Type = type;
	ret->Source = Platform::ReadFile(path);

	Funky::ShaderCompiler::CompileShader(ret);

	return ret;
}
