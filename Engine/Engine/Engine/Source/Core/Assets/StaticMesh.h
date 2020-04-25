#pragma once

#include "Math/Math.h"

#include <unordered_map>

#include "DebugMacros.h"
#include "LogMacros.h"

#include "Core/Utils.h"
#include "Core/Containers.h"

#include "RenderingFrontend/IRenderingResource.h"
#include "RenderingFrontend/VertexDefinition.h"

namespace Funky
{
	class AssetRegistry;

	namespace Asset
	{
		class StaticMesh
		{
			friend class Funky::AssetRegistry;
		public:

			struct Desc
			{
				/** Required */
				FORCEINLINE Hash128 GetHash() const { return HashString(Name); }
				/** Required END */

				Str Name;
				bool bReverseIndices = false;

				Str	Path;
				struct
				{
					darray<Vertex>  Vertices;
					darray<u16>		Indices;
				};

				enum class ECreationType
				{
					Undefined,
					FromSource,
					FromData
				} CreationType = ECreationType::Undefined;
			};

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

			FORCEINLINE Hash128 GetHash() const { return HashString(Name); }

		protected:
			void InitVertices(darray<Vertex> InVertices);
			void InitIndices(darray<u16> InIndices);

		private:
			Str Name;
			darray<Vertex>	Vertices;
			size VerticesSizeInBytes = 0u;

			darray<u16>		Indices;
			size IndicesSizeInBytes = 0u;

			/** In most use cases you PREFER to use this one. */
			[[nodiscard]] 
			static StaticMesh* Create(Str const& Path, bool bReverseIndices = false);

			/** These are leaved here if case of any dynamic generated meshes. */
			[[nodiscard]] 
			static StaticMesh* Create(Str const & Name, darray<Vertex> const& InVertices);
			[[nodiscard]] 
			static StaticMesh* Create(Str const & Name, darray<Vertex> const& InVertices, darray<u16> const& InIndices);

			[[nodiscard]]
			static StaticMesh* CreateFromDesc(Desc const& desc);
		public:
			Rendering::Resource::ID VertexBuffer = Rendering::Resource::ID::Zero;
			Rendering::Resource::ID IndexBuffer  = Rendering::Resource::ID::Zero;
		};
	}
}
