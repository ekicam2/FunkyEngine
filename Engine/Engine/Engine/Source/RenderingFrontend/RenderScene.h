#pragma once

#include "Core/Containers.h"

#include "Light.h"
#define MAX_LIGHTS 125

#include "RenderingFrontend/IRenderingResource.h"

#define MAX_RENDER_PRIMITIVES 10

namespace Funky
{
	namespace Rendering
	{
		/**
		 * RenderPrimitive is a structure representing visible entity.
		 * Main purpose of this struct is to encapsulate ALL data needed by the renderer to draw the entity.
		 */

		struct StaticMesh
		{
			RBuffer * VertexBuffer = nullptr;
			RBuffer * IndexBuffer  = nullptr;
		};

		struct ShaderLink
		{
			RShader * VS = nullptr;
			RShader * PS = nullptr;
		};

		struct RenderPrimitive
		{
			bool bIsValid = false;
			Math::Vec3f	Position = { 0.0f, 0.0f, 0.0f };
			
			StaticMesh Mesh;
			ShaderLink Shaders;
		};

		struct RenderScene
		{
			RenderPrimitive Objects[MAX_RENDER_PRIMITIVES];
		};
	}
}