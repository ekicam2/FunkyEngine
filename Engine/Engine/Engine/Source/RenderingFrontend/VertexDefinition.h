#pragma once

#include "Math/Vector3.h"
#include "Math/Vector2.h"

namespace Funky
{
	struct Vertex
	{
		/*Vertex(DirectX::XMFLOAT3 const & vPos, DirectX::XMFLOAT3 const & vCol) : m_vPosition(vPos), m_vColor(vCol), m_vNormal({ 0.0f, 0.0f, 0.0f }), m_vTexcoords({ 0.0f, 0.0f }) { }
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
		}*/

		Vertex() = default;

		Vertex(
			Math::Vec3f const& InPosition,
			Math::Vec3f const& InColor,
			Math::Vec3f const& InNormal,
			Math::Vec2f const& InTexcoords
		)
			: Position(InPosition)
			, Color(InColor)
			, Normal(InNormal)
			, Texcoords(InTexcoords)
		{
		}

		bool operator==(Vertex const& Other) const
		{
			return Position == Other.Position && Color == Other.Color && Normal == Other.Normal;
		}

		Math::Vec3f Position = Math::Vec3f::Zero;
		Math::Vec3f Color = Math::Vec3f::Zero;
		Math::Vec3f Normal = Math::Vec3f::Zero;
		Math::Vec2f Texcoords = Math::Vec2f::Zero;
	};
}

namespace std {
	template<> struct hash<Funky::Vertex> {
		size_t operator()(Funky::Vertex const& vertex) const {
			auto p1 = std::hash<float>{}(vertex.Position.X);
			auto p2 = std::hash<float>{}(vertex.Position.Y);
			auto p3 = std::hash<float>{}(vertex.Position.Z);

			auto pp = ((p1 ^ (p2 << 1)) >> 1) ^ (p3 << 1);

			auto n1 = std::hash<float>{}(vertex.Normal.X);
			auto n2 = std::hash<float>{}(vertex.Normal.Y);
			auto n3 = std::hash<float>{}(vertex.Normal.Z);

			auto nn = ((n1 ^ (n2 << 1)) >> 1) ^ (n3 << 1);

			auto c1 = std::hash<float>{}(vertex.Color.X);
			auto c2 = std::hash<float>{}(vertex.Color.Y);
			auto c3 = std::hash<float>{}(vertex.Color.Z);

			auto cc = ((c1 ^ (c2 << 1)) >> 1) ^ (c3 << 1);

			auto t1 = std::hash<float>{}(vertex.Texcoords.X);
			auto t2 = std::hash<float>{}(vertex.Texcoords.Y);

			auto tt = (t1 ^ (t2 << 1));

			return ((pp ^ (nn << 1)) >> 1) ^ ((cc << 1) ^ (tt << 1));
		}
	};
}