#pragma once

#include "Math/Math.h"

#include <unordered_map>

#include "DebugMacros.h"
#include "LogMacros.h"

#include "Rendering/RenderingBackend.h"

#include "Core/Containers.h"
#include "Core/Assets/IAsset.h"

namespace Funky
{
	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 const & vPos, DirectX::XMFLOAT3 const & vCol) : m_vPosition(vPos), m_vColor(vCol), m_vNormal({ 0.0f, 0.0f, 0.0f }), m_vTexcoords({ 0.0f, 0.0f }) { }
		Vertex(DirectX::XMFLOAT3 const & vPos, DirectX::XMFLOAT3 const & vCol, DirectX::XMFLOAT2 const & vTexcoord)
			: m_vPosition(vPos), m_vColor(vCol), m_vNormal({ 0.0f, 0.0f, 0.0f }), m_vTexcoords(vTexcoord) { }
		Vertex(DirectX::XMFLOAT3 const & vPos, DirectX::XMFLOAT3 const & vCol, DirectX::XMFLOAT3 const & vNorm, DirectX::XMFLOAT2 const & vTexcoord)
			: m_vPosition(vPos), m_vColor(vCol), m_vNormal(vNorm), m_vTexcoords(vTexcoord) { }
		
		DirectX::XMFLOAT3 m_vPosition;
		DirectX::XMFLOAT3 m_vColor;
		DirectX::XMFLOAT3 m_vNormal;
		DirectX::XMFLOAT2 m_vTexcoords;

		bool operator==(Vertex const & Other) const
		{
			return Other.m_vPosition.x == m_vPosition.x && Other.m_vPosition.y == m_vPosition.y && Other.m_vPosition.z == m_vPosition.z &&
				Other.m_vColor.x == m_vColor.x && Other.m_vColor.y == m_vColor.y && Other.m_vColor.z == m_vColor.z &&
				Other.m_vNormal.x == m_vNormal.x && Other.m_vNormal.y == m_vNormal.y && Other.m_vNormal.z == m_vNormal.z &&
				Other.m_vTexcoords.x == m_vTexcoords.x && Other.m_vTexcoords.y == m_vTexcoords.y;
		}
	};
}

namespace std {
	template<> struct hash<Funky::Vertex> {
		size_t operator()(Funky::Vertex const& vertex) const {
			auto p1 = std::hash<float>{}(vertex.m_vPosition.x);
			auto p2 = std::hash<float>{}(vertex.m_vPosition.y);
			auto p3 = std::hash<float>{}(vertex.m_vPosition.z);

			auto pp = ((p1 ^ (p2 << 1)) >> 1) ^ (p3 << 1);

			auto n1 = std::hash<float>{}(vertex.m_vNormal.x);
			auto n2 = std::hash<float>{}(vertex.m_vNormal.y);
			auto n3 = std::hash<float>{}(vertex.m_vNormal.z);

			auto nn = ((n1 ^ (n2 << 1)) >> 1) ^ (n3 << 1);

			auto c1 = std::hash<float>{}(vertex.m_vColor.x);
			auto c2 = std::hash<float>{}(vertex.m_vColor.y);
			auto c3 = std::hash<float>{}(vertex.m_vColor.z);

			auto cc = ((c1 ^ (c2 << 1)) >> 1) ^ (c3 << 1);

			auto t1 = std::hash<float>{}(vertex.m_vTexcoords.x);
			auto t2 = std::hash<float>{}(vertex.m_vTexcoords.y);

			auto tt = (t1 ^ (t2 << 1));

			return ((pp ^ (nn << 1)) >> 1) ^ ((cc << 1) ^ (tt << 1));
		}
	};
}

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
			RawMesh(str const& Path);
			RawMesh(std::pair<darray<Vertex>, darray<u16>>&& Load);
			virtual ~RawMesh() = default;

			FORCEINLINE size_t GetIndicesCount() const
			{
				return Indices.size();
			}

			FORCEINLINE size_t GetVerticesCount() const
			{
				return Vertices.size();
			}

			FORCEINLINE bool HasValidProxy() const
			{
				return Proxy != Rendering::RenderingBackend::INVALID_INDEX;
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
			friend class MeshUtils;
			friend class Rendering::RenderingBackendImpl;
			friend class Rendering::Renderer; //????!!!

			Rendering::RenderingBackend::MeshProxy Proxy = Rendering::RenderingBackend::INVALID_INDEX;

			darray<Vertex> Vertices;
			darray<u16> Indices;

			inline static u32 TagCounter = 0u;
		};

		//u32 RawMesh::TagCounter = 0u;

	}
}
