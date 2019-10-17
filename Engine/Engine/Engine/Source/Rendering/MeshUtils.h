#pragma once

#include "RawMesh.h"
#include "3rd/TinyObjectLoader.h"
#include "3rd/imgui/imgui.h"
#include "Math/Math.h"

namespace Funky
{
	class MeshUtils
	{
	public: 

		static RawMesh* CreateTerrainPlane(u32 GridCellsX, u32 GridCellsY, f32 StepBetweenGridPoints = 0.0f)
		{
			RawMesh* Returner = new RawMesh();

			const u32 TerrainCellsX = GridCellsX;
			const u32 TerrainCellsY = GridCellsY;

			const f32 xGridOffset = StepBetweenGridPoints ? StepBetweenGridPoints : 1.f / TerrainCellsX;
			const f32 yGridOffset = StepBetweenGridPoints ? StepBetweenGridPoints : 1.f / TerrainCellsY;

			Returner->Vertices.reserve((TerrainCellsY + 1) * (TerrainCellsX + 1));
			Returner->Indices.reserve(TerrainCellsY * TerrainCellsX * 6);

			NoiseGenerator Ng;
			for (u64 y = 0; y < (TerrainCellsY + 1); ++y)
			{
				for (u32 x = 0; x < (TerrainCellsX + 1); ++x)
				{
					f32 NoisedHeight = (f32)(Ng.noise(10.0 * x, 10.0 * y, 1.3) * 2.0);

					f32 absval = (NoisedHeight < 0 ? -NoisedHeight : NoisedHeight);

					Returner->Vertices.push_back(
						Vertex(
							{ x * xGridOffset, NoisedHeight, y *  -yGridOffset },  // pos
							{ absval * 0.1f, 0.0f, 0.0f } // color
						)
					);
				}
			}

			for (u32 y = 0, index = 0; y < TerrainCellsY; ++y, ++index)
			{
				for (u32 x = 0; x < TerrainCellsX; ++x, ++index)
				{
					// first triangle
					Returner->Indices.push_back((u16)(index + (TerrainCellsX + 1) + 1));
					Returner->Indices.push_back((u16)(index + (TerrainCellsX + 1)));
					Returner->Indices.push_back((u16)(index));

					// second triangle
					Returner->Indices.push_back((u16)(index + 1));
					Returner->Indices.push_back((u16)(index + (TerrainCellsX + 1) + 1));
					Returner->Indices.push_back((u16)(index));
				}
			}

			return Returner;
		}
		static RawMesh* CreateCube()
		{
			RawMesh* Returner = new RawMesh();
			Returner->Vertices =
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

			Returner->Indices =
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

			return Returner;
		}
		static RawMesh* CreateCube(float Colors[3], bool CCW = false)
		{
			RawMesh* Returner = new RawMesh();
			Returner->Vertices =
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

			Returner->Indices =
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
				std::reverse(std::begin(Returner->Indices), std::end(Returner->Indices));
			}

			return Returner;
		}
		static RawMesh* LoadOBJFromFile(char const * const pFilePath)
		{
			tinyobj::attrib_t Attribs;
			std::vector<tinyobj::shape_t> Shapes;
			std::vector<tinyobj::material_t> Materials;

			std::string Error, Warnings;

			Timer ParseTimer;
			ParseTimer.Begin();
			const bool bSuccess = tinyobj::LoadObj(&Attribs, &Shapes, &Materials, &Warnings, &Error, pFilePath, NULL, true, false);
			ParseTimer.End();

			LOG_FUNKY(TEXT("Parsing mesh: "), pFilePath, TEXT(". Took: "), ParseTimer.GetInMills(), TEXT(" ms."));

			if (!bSuccess)
			{
				LOG_ERROR_FUNKY(TEXT("Error was: "), Error.c_str());
				BREAK();
				return nullptr;
			}

			RawMesh* Returner = new RawMesh();

			{
				DEF_DEBUG_SCOPE_TIMER(TEXT("Creating mesh"));

				std::unordered_map<Vertex, unsigned> UniqueVerts = {};

				for (u64 ShapeInt = 0; ShapeInt < Shapes.size(); ++ShapeInt)
				{
					for (u64 IndexIndex = Shapes[ShapeInt].mesh.indices.size() - 1; IndexIndex >= 0; --IndexIndex)
					{
						tinyobj::index_t const & Index = Shapes[ShapeInt].mesh.indices[IndexIndex];

						Vertex PropVert = {
							DirectX::XMFLOAT3(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // position
								DirectX::XMFLOAT3(Attribs.vertices[3 * Index.vertex_index], Attribs.vertices[3 * Index.vertex_index + 1], Attribs.vertices[3 * Index.vertex_index + 2]), // colors
								DirectX::XMFLOAT3(Attribs.normals[3 * Index.normal_index], Attribs.normals[3 * Index.normal_index + 1], Attribs.normals[3 * Index.normal_index + 2]), // normal
								DirectX::XMFLOAT2(Attribs.texcoords[2 * Index.texcoord_index], Attribs.texcoords[2 * Index.texcoord_index + 1])
						};

						if (UniqueVerts.count(PropVert) == 0)
						{
							UniqueVerts[PropVert] = static_cast<unsigned>(Returner->Vertices.size());
							Returner->Vertices.push_back(PropVert);
						}

						Returner->Indices.push_back((u16)UniqueVerts[PropVert]);
						//std::reverse(Returner->m_Indices.begin(), Returner->m_Indices.end());
					}
				}
			}

			return Returner;
		}
		static RawMesh* CreateSphere(float Radius, bool CCW = false)
		{
			RawMesh* Returner = new RawMesh();

			const u32 X_SEGMENTS = 12;
			const u32 Y_SEGMENTS = 12;

			Returner->Vertices.reserve(X_SEGMENTS * X_SEGMENTS);

			for (u32 y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (u32 x = 0; x <= X_SEGMENTS; ++x)
				{
					f32 xSegment = (f32)x / (f32)X_SEGMENTS;
					f32 ySegment = (f32)y / (f32)Y_SEGMENTS;
					f32 xPos = Math::Cos(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Funky::Math::PI<f32>) * Radius;
					f32 yPos = Math::Cos(ySegment * Math::PI<f32>) * Radius;
					f32 zPos = Math::Sin(xSegment * 2.0f * Math::PI<f32>) * Math::Sin(ySegment * Funky::Math::PI<f32>) * Radius;

					Returner->Vertices.push_back(
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

					Returner->Indices.push_back((u16)(first + 1));
					Returner->Indices.push_back((u16)(second));
					Returner->Indices.push_back((u16)(first));

					Returner->Indices.push_back((u16)(first + 1));
					Returner->Indices.push_back((u16)(second + 1));
					Returner->Indices.push_back((u16)(second));
				}
			}

			if (CCW)
			{
				std::reverse(std::begin(Returner->Indices), std::end(Returner->Indices));
			}

			return Returner;
		}
	};
}