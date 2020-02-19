#pragma once

#include "Light.h"
#define MAX_LIGHTS 125

namespace Funky
{
	namespace Rendering
	{
		class RenderPrimitive
		{
			RStaticMesh* Mesh;
			RShaderLink* Material;
			Math::Vec3f Position;
		};

		class RenderScene
		{
			RenderPrimitive* SkyDome;
			Light Lights[MAX_LIGHTS];
			darray< RenderPrimitive*> Objects;
		};
	}
}