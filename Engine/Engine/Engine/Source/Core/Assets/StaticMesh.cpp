#include "StaticMesh.h"
#include "Templates.h"
#include "Utils/MeshUtils.h"

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateFromFile(str const& Path)
{
	if (!Path.ends_with(".obj"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadOBJFromFile(Path.c_str());
		return CreateMeshFromRawData(VI.Vertices, VI.Indices);
	}
	else if (!Path.ends_with(".gltf"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadGLTFFromFile(Path.c_str());
		return CreateMeshFromRawData(VI.Vertices, VI.Indices);
	}

	LOG_ERROR("Unsupported mesh file format: ", Path);
	LOG_ERROR("Currently only .obj and .gltf formats are supported.");
	return nullptr;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices)
{
	auto Ret = new StaticMesh();
	Ret->Vertices = InVertices;
	return Ret;
}

Funky::Asset::StaticMesh* Funky::Asset::StaticMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices)
{
	auto Ret = new StaticMesh();
	Ret->Vertices = InVertices;
	Ret->Indices = InIndices;

	return Ret;
}

Funky::Asset::StaticMesh::StaticMesh()
	: IAsset(Asset::EType::Mesh)
{

}
