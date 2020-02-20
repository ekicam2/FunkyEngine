#pragma once

#include "Core/Containers.h"

#include "Light.h"
#define MAX_LIGHTS 125

namespace Funky
{
	namespace Rendering
	{
		/**
		 * RenderPrimitive is a structure representing visible entity.
		 * Main purpose of this struct is to encapsulate ALL data needed by the renderer to draw the entity.
		 */
		struct RenderPrimitive
		{
			RStaticMesh* Mesh;
			RShaderLink* Material;
			Math::Vec3f Position;
		};

		class RenderScene
		{
			RenderPrimitive* SkyDome;
			Light Lights[MAX_LIGHTS];
			darray<RenderPrimitive*> Objects;
		};
	}
}