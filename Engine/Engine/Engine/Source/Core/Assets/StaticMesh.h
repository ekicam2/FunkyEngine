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
			StaticMesh();
			darray<Vertex> Vertices;
			darray<u16> Indices;

			Rendering::RStaticMesh RHandle;
		};
	}
}
