#pragma once

#include "Core/Containers.h"

#include "RenderingFrontend/RenderPrimitive.h"
#include "RenderingFrontend/IRenderingResource.h"

#include "PostProcess.h"

#define MAX_RENDER_PRIMITIVES 10000
#define MAX_POSTPROCESSES 10

namespace Math
{
	class Camera;
}

namespace Funky
{
	namespace Rendering
	{
		struct RenderView
		{
			Math::Camera* Camera = nullptr;
			DirectX::XMMATRIX View;
			DirectX::XMMATRIX Projection;

			DirectX::XMMATRIX ViewProjection;


			RenderPrimitive Objects[MAX_RENDER_PRIMITIVES];
			PostProcess PostProcesses[MAX_POSTPROCESSES];
		};
	}
}