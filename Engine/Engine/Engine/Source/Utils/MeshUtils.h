#pragma once

#include "Core/Containers.h"
#include "RenderingFrontend/VertexDefinition.h"


namespace Funky
{
	class MeshUtils
	{
	public:
		struct VertexIndexBuffer
		{
			darray<Vertex> Vertices;
			darray<u16> Indices;
		};

		static VertexIndexBuffer CreateCube();

		static VertexIndexBuffer CreateTerrainPlane(u32 GridCellsX, u32 GridCellsY, f32 StepBetweenGridPoints = 0.0f);

		static VertexIndexBuffer LoadOBJFromFile(char const* const pFilePath);
		
		static VertexIndexBuffer LoadGLTFFromFile(char const* const pFilePath);

		//static VertexIndexBuffer CreateCube(f32 Colors[3], bool CCW = false);
	};
}
//
//		
//		
//		static VertexIndexBuffer CreateSphere(f32 Radius, bool CCW = false);
//	};
//
//
//	//Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::CreateCube()
//	//{
//	//	MeshLoad Ret;
//	//	auto& [Vertices, Indices] = Ret;
//	//
//	//	Vertices =
//	//	{
//	//		{DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(0,   0,   0)},
//	//		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(0,   0,   1)},
//	//		{DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(0,   1,   0)},
//	//		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0,   1,   1)},
//	//
//	//		{DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(1,   0,   0)},
//	//		{DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(1,   0,   1)},
//	//		{DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(1,   1,   0)},
//	//		{DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1,   1,   1)},
//	//	};
//	//
//	//	Indices =
//	//	{
//	//		1,2,0, // -x
//	//		3,2,1,
//	//
//	//		6,5,4, // +x
//	//		6,7,5,
//	//
//	//		5,1,0, // -y
//	//		4,5,0,
//	//
//	//		7,6,2, // +y
//	//		3,7,2,
//	//
//	//		6,4,0, // -z
//	//		2,6,0,
//	//
//	//		7,3,1, // +z
//	//		5,7,1,
//	//	};
//	//
//	//	return Ret;
//	//}
//
//	MeshUtils::VertexIndexBuffer MeshUtils::LoadOBJFromFile(char const* const pFilePath)
//	{
//		tinyobj::attrib_t Attribs;
//		std::vector<tinyobj::shape_t> Shapes;
//		std::vector<tinyobj::material_t> Materials;
//
//		std::string Error, Warnings;
//
//		Timer ParseTimer;
//		ParseTimer.Begin();
//		const bool bSuccess = tinyobj::LoadObj(&Attribs, &Shapes, &Materials, &Warnings, &Error, pFilePath, NULL, true, false);
//		ParseTimer.End();
//
//		LOG(TEXT("Parsing mesh: "), pFilePath, TEXT(". Took: "), ParseTimer.GetInMills(), TEXT(" ms."));
//
//		if (!bSuccess)
//		{
//			LOG_ERROR(TEXT("Error was: "), Error.c_str());
//			BREAK();
//			return {};
//		}
//
//		VertexIndexBuffer Ret;
//		auto& [Vertices, Indices] = Ret;
//		{
//			DEF_DEBUG_SCOPE_TIMER(TEXT("Creating mesh"));
//
//			std::unordered_map<Vertex, unsigned> UniqueVerts = {};
//
//			for (u64 ShapeInt = 0; ShapeInt < Shapes.size(); ++ShapeInt)
//			{
//				for (i64 IndexIndex = Shapes[ShapeInt].mesh.indices.size() - 1; IndexIndex >= 0; --IndexIndex)
//				{
//					tinyobj::index_t const& Index = Shapes[ShapeInt].mesh.indices[IndexIndex];
//
//					Vertex PropVert = {
//						Math::Vec3f(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // position
//						Math::Vec3f(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // colors
//						Math::Vec3f(Attribs.normals[3 * Index.normal_index], Attribs.normals[3 * Index.normal_index + 1], Attribs.normals[3 * Index.normal_index + 2]), // normal
//						Math::Vec2f(Attribs.texcoords[2 * Index.texcoord_index], Attribs.texcoords[2 * Index.texcoord_index + 1])
//					};
//
//					if (UniqueVerts.count(PropVert) == 0)
//					{
//						UniqueVerts[PropVert] = static_cast<unsigned>(Vertices.size());
//						Vertices.push_back(PropVert);
//					}
//
//					Indices.push_back((u16)UniqueVerts[PropVert]);
//					//std::reverse(Returner->m_Indices.begin(), Returner->m_Indices.end());
//				}
//			}
//		}
//
//		return Ret;
//	}
//
//	MeshUtils::VertexIndexBuffer MeshUtils::LoadGLTFFromFile(char const* const pFilePath)
//	{
//		pFilePath;
//		return {};
//	}
//
//	MeshUtils::VertexIndexBuffer MeshUtils::CreateSphere(f32 Radius, bool CCW /*= false*/)
//	{
//		VertexIndexBuffer Ret;
//		auto& [Vertices, Indices] = Ret;
//
//		const u32 X_SEGMENTS = 12;
//		const u32 Y_SEGMENTS = 12;
//
//		Vertices.reserve(X_SEGMENTS * X_SEGMENTS);
//
//		for (u32 y = 0; y <= Y_SEGMENTS; ++y)
//		{
//			for (u32 x = 0; x <= X_SEGMENTS; ++x)
//			{
//				f32 xSegment = (f32)x / (f32)X_SEGMENTS;
//				f32 ySegment = (f32)y / (f32)Y_SEGMENTS;
//				f32 xPos = Math::Cos(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Math::PI<f32>) * Radius;
//				f32 yPos = Math::Cos(ySegment * Math::PI<f32>) * Radius;
//				f32 zPos = Math::Sin(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Math::PI<f32>) * Radius;
//
//				Vertices.push_back(
//					Vertex(
//						Math::Vec3f(xPos, yPos, zPos),
//						Math::Vec3f(xPos / Radius, yPos / Radius, zPos / Radius),
//						Math::Vec3f(-xPos, -yPos, -zPos),
//						Math::Vec2f(xSegment, ySegment)
//					)
//				);
//			}
//		}
//
//		for (u64 latNumber = 0; latNumber < Y_SEGMENTS; latNumber++)
//		{
//			for (u64 longNumber = 0; longNumber < X_SEGMENTS; longNumber++)
//			{
//				u64 first = (latNumber * (X_SEGMENTS + 1)) + longNumber;
//				u64 second = first + X_SEGMENTS + 1;
//
//				Indices.push_back((u16)(first + 1));
//				Indices.push_back((u16)(second));
//				Indices.push_back((u16)(first));
//
//				Indices.push_back((u16)(first + 1));
//				Indices.push_back((u16)(second + 1));
//				Indices.push_back((u16)(second));
//			}
//		}
//
//		if (CCW)
//		{
//			std::reverse(std::begin(Indices), std::end(Indices));
//		}
//
//		return Ret;
//	}
//
//	//Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::CreateCube(f32 Colors[3], bool CCW /*= false*/)
//	//{
//	//	VertexIndexBuffer Ret;
//	//	auto& [Vertices, Indices] = Ret;
//	//
//	//	Vertices =
//	//	{
//	//		Vertex(Math::Vec3f(-0.5f,-0.5f,-0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])),
//	//		Vertex(Math::Vec3f(-0.5f,-0.5f, 0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])),
//	//		Vertex(Math::Vec3f(-0.5f, 0.5f,-0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])),
//	//		Vertex(Math::Vec3f(-0.5f, 0.5f, 0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])),
//	//		
//	//		Vertex{Math::Vec3f(0.5f,-0.5f,-0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])},
//	//		Vertex{Math::Vec3f(0.5f,-0.5f, 0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])},
//	//		Vertex{Math::Vec3f(0.5f, 0.5f,-0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])},
//	//		Vertex{Math::Vec3f(0.5f, 0.5f, 0.5f), Math::Vec3f(Colors[0], Colors[1], Colors[2])},
//	//	};
//	//
//	//	Indices =
//	//	{
//	//		1,2,0, // -x
//	//		3,2,1,
//	//
//	//		6,5,4, // +x
//	//		6,7,5,
//	//
//	//		5,1,0, // -y
//	//		4,5,0,
//	//
//	//		7,6,2, // +y
//	//		3,7,2,
//	//
//	//		6,4,0, // -z
//	//		2,6,0,
//	//
//	//		7,3,1, // +z
//	//		5,7,1,
//	//	};
//	//
//	//	if (CCW)
//	//	{
//	//		std::reverse(std::begin(Indices), std::end(Indices));
//	//	}
//	//
//	//	return Ret;
//	//}
//
//
//
//}