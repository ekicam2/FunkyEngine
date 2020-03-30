#include "DX11RenderingResources.h"



Funky::Rendering::DX11VertexShader::DX11VertexShader()
{
	ShaderType = RShader::Type::Vertex;
}

Funky::Rendering::DX11PixelShader::DX11PixelShader()
{
	ShaderType = RShader::Type::Pixel;
}
