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
		public:
			Math::Vec2u Size = Math::Vec2u(0, 0);
		};

		class RDepthStencil : public IRenderingResource
		{

		};

		class RBuffer : public IRenderingResource
		{
		public:
			using Data_t = void const * const;
			enum class EType : u8
			{
				None,
				Vertex,
				Index,
				Uniform
			};

			enum class EUsageType : u8
			{
				Default,
				Static,
				Dynamic,
				Staging
			};

			RBuffer() = default;
			
			EType BufferType = EType::None;
			EUsageType Usage = EUsageType::Default;
			u32 ElementCount = 0u;
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

	}
}