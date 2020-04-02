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
			// IAsset
			virtual ~StaticMesh() = default;
			// IAsset END
			
			/** In most use cases you PREFER to use this one. */
			[[nodiscard]] static StaticMesh* CreateFromFile(str const& Path, bool bReverseIndices = false);

			/** These are leaved here if case of any dynamic generated meshes. */
			[[nodiscard]] static StaticMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices);
			[[nodiscard]] static StaticMesh* CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices);


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
				return VerticesSizeInBytes;
			}

			FORCEINLINE size GetIndexBufferSizeInBytes() const
			{
				return IndicesSizeInBytes;
			}

			FORCEINLINE Vertex const * GetVertices() const
			{
				return Vertices.data();
			}

			FORCEINLINE u16 const * GetIndices() const
			{
				return Indices.data();
			}

		protected:
			void InitVertices(darray<Vertex> InVertices);
			void InitIndices(darray<u16> InIndices);

		private:
			StaticMesh();

			darray<Vertex>	Vertices;
			size VerticesSizeInBytes = 0u;

			darray<u16>		Indices;
			size IndicesSizeInBytes = 0u;

			//todo rethink
		public:
			Rendering::RBuffer * VertexBuffer = nullptr;
			Rendering::RBuffer * IndexBuffer  = nullptr;
		};
	}
}
