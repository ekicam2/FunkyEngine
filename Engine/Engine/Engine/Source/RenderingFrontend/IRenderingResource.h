#pragma once

#include "Core/Containers.h"

#include "RenderingFrontend/VertexDefinition.h"

namespace Funky
{
	namespace Rendering
	{
		class IRenderingResource
		{
		public:
			virtual ~IRenderingResource() = default;
		};

		class RStaticMesh : public IRenderingResource
		{
		public:
			RStaticMesh() {}
			
			bool Initilize(
				[[maybe_unused]] class RenderingBackend* InRenderingBackend, 
				[[maybe_unused]] darray<Vertex> const & Vertices, 
				[[maybe_unused]] darray<u16> const & Indices
			)
			{

			}

		private:
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
	}
}