#pragma once

#include "Utils/MeshUtils.h"
#include "3rd/TinyObjectLoader.h"
#include "Math/Math.h"

#include "Core/Timer.h"
#include "Utils/NoiseGenerator.h"

MeshLoad Funky::MeshUtils::CreateTerrainPlane(u32 GridCellsX, u32 GridCellsY, f32 StepBetweenGridPoints /*= 0.0f*/)
{
	MeshLoad Ret;
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
					{ absval * 0.1f, 0.0f, 0.0f } // color
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

MeshLoad Funky::MeshUtils::CreateCube()
{
	MeshLoad Ret;
	auto& [Vertices, Indices] = Ret;

	Vertices =
	{
		{DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(0,   0,   0)},
		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(0,   0,   1)},
		{DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(0,   1,   0)},
		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0,   1,   1)},

		{DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(1,   0,   0)},
		{DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(1,   0,   1)},
		{DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(1,   1,   0)},
		{DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1,   1,   1)},
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

MeshLoad Funky::MeshUtils::LoadOBJFromFile(char const* const pFilePath)
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

	MeshLoad Ret;
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
					DirectX::XMFLOAT3(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // position
						DirectX::XMFLOAT3(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // colors
						DirectX::XMFLOAT3(Attribs.normals[3 * Index.normal_index], Attribs.normals[3 * Index.normal_index + 1], Attribs.normals[3 * Index.normal_index + 2]), // normal
						DirectX::XMFLOAT2(Attribs.texcoords[2 * Index.texcoord_index], Attribs.texcoords[2 * Index.texcoord_index + 1])
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

MeshLoad Funky::MeshUtils::CreateSphere(f32 Radius, bool CCW /*= false*/)
{
	MeshLoad Ret;
	auto& [Vertices, Indices] = Ret;

	const u32 X_SEGMENTS = 12;
	const u32 Y_SEGMENTS = 12;

	Vertices.reserve(X_SEGMENTS * X_SEGMENTS);

	for (u32 y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (u32 x = 0; x <= X_SEGMENTS; ++x)
		{
			f32 xSegment = (f32)x / (f32)X_SEGMENTS;
			f32 ySegment = (f32)y / (f32)Y_SEGMENTS;
			f32 xPos = Math::Cos(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Math::PI<f32>) * Radius;
			f32 yPos = Math::Cos(ySegment * Math::PI<f32>) * Radius;
			f32 zPos = Math::Sin(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Math::PI<f32>) * Radius;

			Vertices.push_back(
				{ DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(xPos / Radius, yPos / Radius, zPos / Radius), DirectX::XMFLOAT2(xSegment, ySegment) }
			);
		}
	}

	for (u64 latNumber = 0; latNumber < Y_SEGMENTS; latNumber++)
	{
		for (u64 longNumber = 0; longNumber < X_SEGMENTS; longNumber++)
		{
			u64 first = (latNumber * (X_SEGMENTS + 1)) + longNumber;
			u64 second = first + X_SEGMENTS + 1;

			Indices.push_back((u16)(first + 1));
			Indices.push_back((u16)(second));
			Indices.push_back((u16)(first));

			Indices.push_back((u16)(first + 1));
			Indices.push_back((u16)(second + 1));
			Indices.push_back((u16)(second));
		}
	}

	if (CCW)
	{
		std::reverse(std::begin(Indices), std::end(Indices));
	}

	return Ret;
}

MeshLoad Funky::MeshUtils::CreateCube(f32 Colors[3], bool CCW /*= false*/)
{
	MeshLoad Ret;
	auto& [Vertices, Indices] = Ret;

	Vertices =
	{
		{DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},

		{DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
		{DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(Colors[0], Colors[1], Colors[2])},
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

	if (CCW)
	{
		std::reverse(std::begin(Indices), std::end(Indices));
	}

	return Ret;
}
