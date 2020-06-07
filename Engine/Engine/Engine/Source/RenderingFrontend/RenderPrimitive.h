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

			Math::Vec3f	Position	= { 0.0f, 0.0f, 0.0f };
			Math::Vec3f	Rotation	= { 0.0f, 0.0f, 0.0f };
			f32			Scale		= 1.0f;
			
			DirectX::XMMATRIX ParentModel = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX GetModelMatrixNoScale()
			{
				DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(
					Math::ToRad(Rotation.X),
					Math::ToRad(Rotation.Y),
					Math::ToRad(Rotation.Z)
				);
				model *= DirectX::XMMatrixTranslation(Position.X, Position.Y, Position.Z);
				model *= DirectX::XMMatrixScaling(Scale, Scale, Scale);

				return model;
			}

			StaticMesh Mesh;
			ShaderLink Shaders;

			i32 ChildrenCount = 0;
		};
	}
}