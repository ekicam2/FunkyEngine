#pragma once

#include "RenderingBackend.h"
#include "Core/Containers.h"

namespace Funky
{
	struct Material
	{
		Rendering::RenderingBackend::VertexShader VS = Rendering::RenderingBackend::INVALID_INDEX;
		Rendering::RenderingBackend::PixelShader PS = Rendering::RenderingBackend::INVALID_INDEX;
		darray<Rendering::RenderingBackend::ConstantBuffer> CB;
	};
}
