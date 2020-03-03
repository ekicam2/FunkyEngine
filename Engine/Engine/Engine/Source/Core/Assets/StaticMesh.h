#pragma once

#include "Math/Math.h"

#include <unordered_map>

#include "DebugMacros.h"
#include "LogMacros.h"

#include "Core/Containers.h"
#include "Core/Assets/IAsset.h"

#include "RenderingFrontend/IRenderingResource.h"
#include "RenderingFrontend/VertexDefinition.h"

namespace Funky
{
	namespace Asset
	{
		class StaticMesh : public IAsset
		{
		public:

			static StaticMesh* CreateFromFile(str const& Path);
			static StaticMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices);
			static StaticMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices);

			virtual ~StaticMesh() = default;

			FORCEINLINE size GetIndicesCount() const
			{
				return Indices.size();
			}

			FORCEINLINE size GetVerticesCount() const
			{
				return Vertices.size();
			}

			FORCEINLINE size GetVertexBufferSizeInBytes() const
			{
				return Vertices.size() * sizeof(Vertex);
			}

			FORCEINLINE size GetIndexBufferSizeInBytes() const
			{
				return Indices.size() * sizeof(u16);
			}

			FORCEINLINE Vertex const * GetVertices() const
			{
				return Vertices.data();
			}

			FORCEINLINE u16 const * GetIndices() const
			{
				return Indices.data();
			}

		private:
			StaticMesh();

			darray<Vertex>	Vertices;
			darray<u16>		Indices;

			//todo rethink
		public:
			Rendering::RBuffer * VertexBuffer = nullptr;
			Rendering::RBuffer * IndexBuffer  = nullptr;
		};
	}
}
