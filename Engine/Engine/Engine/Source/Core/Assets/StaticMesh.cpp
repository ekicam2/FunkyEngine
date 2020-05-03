#include "StaticMesh.h"
#include "Templates.h"
#include "Utils/MeshUtils.h"

#include "Core/Timer.h"

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::Create(Str const& Path, bool bReverseIndices /*= false*/)
{
	if (Path.EndsWith(".obj"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadOBJFromFile(Path.GetBuffer(), bReverseIndices);
		return Create(Path, VI.Vertices, VI.Indices);
	}
	else if (Path.EndsWith(".gltf"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadGLTFFromFile(Path.GetBuffer());
		return Create(Path, VI.Vertices, VI.Indices);
	}

	LOG_ERROR("Unsupported mesh file format: ", Path);
	LOG_ERROR("Currently only .obj and .gltf formats are supported.");
	return nullptr;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::Create(Str const & Name, darray<Vertex> const& InVertices)
{
	auto Ret = new StaticMesh();
	Ret->Name = Name;
	Ret->InitVertices(InVertices);
	return Ret;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::Create(Str const& Name, darray<Vertex> const& InVertices, darray<u16> const& InIndices)
{
	auto Ret = Create(Name, InVertices);
	Ret->InitIndices(InIndices);
	return Ret;
}

void Funky::Asset::StaticMesh::InitVertices(darray<Vertex> InVertices)
{
	Vertices = InVertices;
	VerticesSizeInBytes = Vertices.size() * sizeof(Vertex);
	{
		DEBUG_SCOPE_TIMER(TEXT("Creating AABB"));

		Math::Vec3f min = Math::Vec3f::Zero;
		Math::Vec3f max = Math::Vec3f::Zero;

		for (auto& current : Vertices)
		{
			min.X = Math::Min(min.X, current.Position.X);
			min.Y = Math::Min(min.Y, current.Position.Y);
			min.Z = Math::Min(min.Z, current.Position.Z);

			max.X = Math::Max(max.X, current.Position.X);
			max.Y = Math::Max(max.Y, current.Position.Y);
			max.Z = Math::Max(max.Z, current.Position.Z);
		}

		BoundingBox.Min = min;
		BoundingBox.Max = max;
	}
}

void Funky::Asset::StaticMesh::InitIndices(darray<u16> InIndices)
{
	Indices = InIndices;
	IndicesSizeInBytes = Indices.size() * sizeof(u16);
}
