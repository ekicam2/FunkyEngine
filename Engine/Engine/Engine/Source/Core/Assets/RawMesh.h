#pragma once

#include "Math/Math.h"

#include <unordered_map>

#include "DebugMacros.h"
#include "LogMacros.h"

#include "Core/Containers.h"
#include "Core/Assets/IAsset.h"

#include "RenderingFrontend/VertexDefinition.h"

namespace Funky
{
	namespace Rendering
	{
		class RenderingBackendImpl;
		class Renderer;
	}

	namespace Asset
	{
		class RawMesh : public IAsset
		{
		public:

			static RawMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices);
			static RawMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices);

			RawMesh(str const& Path);
			virtual ~RawMesh() = default;

			FORCEINLINE size_t GetIndicesCount() const
			{
				return Indices.size();
			}

			FORCEINLINE size_t GetVerticesCount() const
			{
				return Vertices.size();
			}

			FORCEINLINE darray<Vertex> const & GetVertices() const
			{
				return Vertices;
			}

			FORCEINLINE darray<u16> const& GetIndices() const
			{
				return Indices;
			}

		private:
			RawMesh();
			darray<Vertex> Vertices;
			darray<u16> Indices;
		};
	}
}
