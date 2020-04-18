#include "Utils/MeshUtils.h"

#include "Math/Math.h"
#include "Core/Timer.h"

#include "3rd/TinyObjectLoader.h"
#include "3rd/TinyGLTFLoader.h"

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

Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::LoadOBJFromFile(char const* const pFilePath, bool bReverseIndices)
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
		DEBUG_SCOPE_TIMER(TEXT("Creating mesh"));

		std::unordered_map<Vertex, unsigned> UniqueVerts = {};

		for (u64 ShapeIndex = 0; ShapeIndex < Shapes.size(); ++ShapeIndex)
		{
			for (i64 IndexIndex = Shapes[ShapeIndex].mesh.indices.size() - 1; IndexIndex >= 0; --IndexIndex)
			{
				tinyobj::index_t const& Index = Shapes[ShapeIndex].mesh.indices[IndexIndex];

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

	if(bReverseIndices)
		std::reverse(Indices.begin(), Indices.end());

	return Ret;
}

Funky::MeshUtils::VertexIndexBuffer Funky::MeshUtils::LoadGLTFFromFile(char const* const pFilePath)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;

	std::string err;
	std::string warn;

	Timer ParseTimer;
	ParseTimer.Begin();
	const bool success = loader.LoadASCIIFromFile(&model, &err, &warn, pFilePath);
	ParseTimer.End();

	LOG(TEXT("Parsing mesh: "), pFilePath, TEXT(". Took: "), ParseTimer.GetInMills(), TEXT(" ms."));

	if (!warn.empty()) 
	{
		LOG_WARNING(warn);
	}

	if (!err.empty()) 
	{
		LOG_ERROR(err);
	}

	if (!success) 
	{
		LOG_ERROR("Failed to parse glTF.");
		return {};
	}

	VertexIndexBuffer Ret;
	auto& [Vertices, Indices] = Ret;

	for (auto& primitive : model.meshes[0].primitives)
	{
		{
			const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes["POSITION"]];
			const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];

			const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
			const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

			Vertices.reserve(posAccessor.count);
			CHECK(Vertices.size() == 0 && Vertices.capacity() == posAccessor.count);

			for (size_t i = 0; i < posAccessor.count; ++i)
			{
				Vertices.push_back(
					Vertex(
						Math::Vec3f(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]), // position
						Math::Vec3f(0.0f, 0.0f, 0.0f), // colors
						Math::Vec3f(0.0f, 0.0f, 0.0f), // normal
						Math::Vec2f(0.0f, 0.0f) // uv
					)
				);
			}
		}

		{
			const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
			const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];

			const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
			const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

			for (size_t i = 0; i < normalAccessor.count; ++i)
			{
				Vertices[i].Normal.X = normals[i * 3 + 0];
				Vertices[i].Normal.Y = normals[i * 3 + 1];
				Vertices[i].Normal.Z = normals[i * 3 + 2];
			}
		}

		{
			const tinygltf::Accessor& uvsAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
			const tinygltf::BufferView& uvsBufferView = model.bufferViews[uvsAccessor.bufferView];

			const tinygltf::Buffer& uvsBuffer = model.buffers[uvsBufferView.buffer];
			const float* uvs = reinterpret_cast<const float*>(&uvsBuffer.data[uvsBufferView.byteOffset + uvsAccessor.byteOffset]);

			for (size_t i = 0; i < uvsAccessor.count; ++i)
			{
				Vertices[i].Texcoords.X = uvs[i * 2 + 0];
				Vertices[i].Texcoords.Y = uvs[i * 2 + 1];
			}
		}

		{
			const tinygltf::Accessor& indAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indBufferView = model.bufferViews[indAccessor.bufferView];

			const tinygltf::Buffer& indBuffer = model.buffers[indBufferView.buffer];
			const u16* indices = reinterpret_cast<const u16*>(&indBuffer.data[indBufferView.byteOffset + indAccessor.byteOffset]);

			Indices.reserve(indAccessor.count);
			CHECK(Indices.size() == 0 && Indices.capacity() == indAccessor.count);

			for (size_t i = 0; i < indAccessor.count; ++i)
			{
				Indices[i] = indices[i];
			}

		}
	}

	return Ret;
}
