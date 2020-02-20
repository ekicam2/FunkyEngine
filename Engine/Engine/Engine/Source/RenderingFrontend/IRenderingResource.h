#pragma once

#include "Core/Containers.h"

#include "RenderingFrontend/VertexDefinition.h"

namespace Funky
{
	namespace Rendering
	{
		/*
			We need to represent new type for accessing rendering resources only.
			Main concept behind that is to split responsibility of memory management.
			Every RenderingResource is GPU data state representation and should not be accessed from other thread than rendering one.

			StaticMesh (asset) -> RStaticMesh
			Texture (asset) -> RTexture
			etc.

			every IRenderingAsset should be registered in RenderingResourcesManager so we can track all of items from one place.
		*/
		class IRenderingResource
		{
		public:
			virtual ~IRenderingResource() = default;
		};

		class RRenderTarget : public IRenderingResource
		{

		};

		class RDepthStencil : public IRenderingResource
		{

		};

		class RBuffer : public IRenderingResource
		{
		public:
			using Data_t = void*;
			enum class Type : u8
			{
				None,
				Vertex,
				Index,
				Uniform
			};

			enum class UsageType : u8
			{
				Default,
				Static,
				Dynamic,
				Staging
			};

			RBuffer() = default;
			RBuffer(Type NewType, UsageType NewUsageType) : BufferType(NewType), Usage(NewUsageType) {}
			Type GetType() const { return BufferType; }
		private:
			Type BufferType = Type::None;
			UsageType Usage = UsageType::Default;
		};

		class RShader : public IRenderingResource
		{
		public:
			enum class Type
			{
				None,
				Vertex,
				Pixel,
				Compute
			} ShaderType;
		};

		class RTexture : public IRenderingResource
		{
			enum Type
			{
				Texture2D,
				Texture3D
			} TextureType;

		};

		/** DIMMIES */

		struct RStaticMesh
		{
			RBuffer* VertexBuffer;
			RBuffer* IndexBuffer;
		};

		struct RShaderLink
		{
			RShader* VS;
			RShader* PS;
		};

	}
}