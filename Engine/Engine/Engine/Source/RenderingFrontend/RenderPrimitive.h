#pragma once

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
			RBuffer* VertexBuffer = nullptr;
			RBuffer* IndexBuffer = nullptr;
		};

		struct ShaderLink
		{
			RShader* VS = nullptr;
			RShader* PS = nullptr;
		};

		struct RenderPrimitive
		{
			bool bIsValid = false;
			Math::Vec3f	Position = { 0.0f, 0.0f, 0.0f };
			Math::Vec3f	Rotation = { 0.0f, 0.0f, 0.0f };

			StaticMesh Mesh;
			ShaderLink Shaders;
		};
	}
}