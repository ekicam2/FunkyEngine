#include "StaticMesh.h"
#include "Templates.h"
#include "Utils/MeshUtils.h"

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateFromFile(Str const& Path, bool bReverseIndices /*= false*/)
{
	if (!Path.EndsWith(".obj"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadOBJFromFile(Path.GetBuffer(), bReverseIndices);
		return CreateMeshFromRawData(VI.Vertices, VI.Indices);
	}
	else if (!Path.EndsWith(".gltf"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadGLTFFromFile(Path.GetBuffer());
		return CreateMeshFromRawData(VI.Vertices, VI.Indices);
	}

	LOG_ERROR("Unsupported mesh file format: ", Path);
	LOG_ERROR("Currently only .obj and .gltf formats are supported.");
	return nullptr;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices)
{
	auto Ret = new StaticMesh();
	Ret->InitVertices(InVertices);
	return Ret;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices)
{
	auto Ret = new StaticMesh();
	Ret->InitVertices(InVertices);
	Ret->InitIndices(InIndices);
	return Ret;
}

void Funky::Asset::StaticMesh::InitVertices(darray<Vertex> InVertices)
{
	Vertices = InVertices;
	VerticesSizeInBytes = Vertices.size() * sizeof(Vertex);
}

void Funky::Asset::StaticMesh::InitIndices(darray<u16> InIndices)
{
	Indices = InIndices;
	IndicesSizeInBytes = Indices.size() * sizeof(u16);
}

Funky::Asset::StaticMesh::StaticMesh()
	: IAsset(Asset::EAssetType::Mesh)
{

}
