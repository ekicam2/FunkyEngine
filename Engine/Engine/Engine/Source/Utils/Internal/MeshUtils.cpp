#include "Utils/MeshUtils.h"

#include "Math/Math.h"
#include "Core/Timer.h"
#include "3rd/TinyObjectLoader.h"
#include "Utils/NoiseGenerator.h"
#include "DebugMacros.h"


Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::CreateCube()
{
	VertexIndexBuffer Ret;
	auto& [Vertices, Indices] = Ret;
		
	Vertices =
	{
		Vertex(Math::Vec3f(-0.5f,-0.5f,-0.5f), Math::Vec3f(0,   0,   0), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(-0.5f,-0.5f, 0.5f), Math::Vec3f(0,   0,   1), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(-0.5f, 0.5f,-0.5f), Math::Vec3f(0,   1,   0), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(-0.5f, 0.5f, 0.5f), Math::Vec3f(0,   1,   1), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		
		Vertex(Math::Vec3f(0.5f,-0.5f,-0.5f), Math::Vec3f(1,   0,   0), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(0.5f,-0.5f, 0.5f), Math::Vec3f(1,   0,   1), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(0.5f, 0.5f,-0.5f), Math::Vec3f(1,   1,   0), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
		Vertex(Math::Vec3f(0.5f, 0.5f, 0.5f), Math::Vec3f(1,   1,   1), Math::Vec3f(0.0f, 0.0f, 0.0f), Math::Vec2f(0.0f, 0.0f)),
	};
		
	Indices =
	{
		1,2,0, // -x
		3,2,1,
		
		6,5,4, // +x
		6,7,5,
		
		5,1,0, // -y
		4,5,0,
		
		7,6,2, // +y
		3,7,2,
		
		6,4,0, // -z
		2,6,0,
		
		7,3,1, // +z
		5,7,1,
	};
		
	return Ret;
}

Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::CreateTerrainPlane(u32 GridCellsX, u32 GridCellsY, f32 StepBetweenGridPoints /*= 0.0f*/)
{
	VertexIndexBuffer Ret;
	auto& [Vertices, Indices] = Ret;
	
	const u64 TerrainCellsX = GridCellsX;
	const u64 TerrainCellsY = GridCellsY;
	
	const f32 xGridOffset = StepBetweenGridPoints ? StepBetweenGridPoints : 1.f / TerrainCellsX;
	const f32 yGridOffset = StepBetweenGridPoints ? StepBetweenGridPoints : 1.f / TerrainCellsY;
	
	Vertices.reserve((TerrainCellsY + 1u) * (TerrainCellsX + 1u));
	Indices.reserve(TerrainCellsY * TerrainCellsX * 6u);
	
	Utils::NoiseGenerator Ng;
	for (u64 y = 0; y < (TerrainCellsY + 1u); ++y)
	{
		for (u32 x = 0; x < (TerrainCellsX + 1); ++x)
		{
			f32 NoisedHeight = (f32)(Ng.noise(10.0 * x, 10.0 * y, 1.3) * 2.0);
	
			f32 absval = (NoisedHeight < 0 ? -NoisedHeight : NoisedHeight);
	
			Vertices.push_back(
				Vertex(
					{ x * xGridOffset, NoisedHeight, y * -yGridOffset },  // pos
					{ absval * 0.1f, 0.0f, 0.0f }, // color
					{ 0.0f, 0.0f, 0.0f }, // normal
					{ 0.0f, 0.0f } //uv
				)
			);
		}
	}
	
	for (u32 y = 0u, index = 0; y < TerrainCellsY; ++y, ++index)
	{
		for (u32 x = 0; x < TerrainCellsX; ++x, ++index)
		{
			// first triangle
			Indices.push_back((u16)(index + (TerrainCellsX + 1) + 1));
			Indices.push_back((u16)(index + (TerrainCellsX + 1)));
			Indices.push_back((u16)(index));
	
			// second triangle
			Indices.push_back((u16)(index + 1));
			Indices.push_back((u16)(index + (TerrainCellsX + 1) + 1));
			Indices.push_back((u16)(index));
		}
	}
	
	return Ret;
}

Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::LoadOBJFromFile(char const* const pFilePath)
{
	tinyobj::attrib_t Attribs;
	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> Materials;

	std::string Error, Warnings;

	Timer ParseTimer;
	ParseTimer.Begin();
	const bool bSuccess = tinyobj::LoadObj(&Attribs, &Shapes, &Materials, &Warnings, &Error, pFilePath, NULL, true, false);
	ParseTimer.End();

	LOG(TEXT("Parsing mesh: "), pFilePath, TEXT(". Took: "), ParseTimer.GetInMills(), TEXT(" ms."));

	if (!bSuccess)
	{
		LOG_ERROR(TEXT("Error was: "), Error.c_str());
		BREAK();
		return {};
	}

	VertexIndexBuffer Ret;
	auto& [Vertices, Indices] = Ret;
	{
		DEF_DEBUG_SCOPE_TIMER(TEXT("Creating mesh"));

		std::unordered_map<Vertex, unsigned> UniqueVerts = {};

		for (u64 ShapeInt = 0; ShapeInt < Shapes.size(); ++ShapeInt)
		{
			for (i64 IndexIndex = Shapes[ShapeInt].mesh.indices.size() - 1; IndexIndex >= 0; --IndexIndex)
			{
				tinyobj::index_t const& Index = Shapes[ShapeInt].mesh.indices[IndexIndex];

				Vertex PropVert = {
					Math::Vec3f(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // position
					Math::Vec3f(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // colors
					Math::Vec3f(Attribs.normals[3 * Index.normal_index], Attribs.normals[3 * Index.normal_index + 1], Attribs.normals[3 * Index.normal_index + 2]), // normal
					Math::Vec2f(Attribs.texcoords[2 * Index.texcoord_index], Attribs.texcoords[2 * Index.texcoord_index + 1])
				};

				if (UniqueVerts.count(PropVert) == 0)
				{
					UniqueVerts[PropVert] = static_cast<unsigned>(Vertices.size());
					Vertices.push_back(PropVert);
				}

				Indices.push_back((u16)UniqueVerts[PropVert]);
				//std::reverse(Returner->m_Indices.begin(), Returner->m_Indices.end());
			}
		}
	}

	return Ret;
}

Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::LoadGLTFFromFile(char const* const pFilePath)
{
	pFilePath;
	return {};
}
