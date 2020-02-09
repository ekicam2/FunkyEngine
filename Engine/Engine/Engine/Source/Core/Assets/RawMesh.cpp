#include "RawMesh.h"
#include "Templates.h"
#include "Utils/MeshUtils.h"

Funky::Asset::RawMesh* Funky::Asset::RawMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices)
{
	auto Ret = new RawMesh();
	Ret->Vertices = InVertices;
	return Ret;
}

Funky::Asset::RawMesh* Funky::Asset::RawMesh::CreateMeshFromRawData(darray<Vertex> const& InVertices, darray<u16> const& InIndices)
{
	auto Ret = new RawMesh();
	Ret->Vertices = InVertices;
	Ret->Indices = InIndices;

	return Ret;
}

Funky::Asset::RawMesh::RawMesh(str const& Path)
	: IAsset(Path, Asset::EType::Mesh)
{
	/*if (!Path.ends_with(".obj"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadOBJFromFile(Path.c_str());
		Vertices = Funky::Move(VI.Vertices);
		Indices = Funky::Move(VI.Indices);
		
		return;
	}
	else if (!Path.ends_with(".gltf"))
	{
		Funky::MeshUtils::VertexIndexBuffer VI = MeshUtils::LoadGLTFFromFile(Path.c_str());
		Vertices = Funky::Move(VI.Vertices);
		Indices = Funky::Move(VI.Indices);

		return;
	}*/

	LOG_ERROR("Unsupported mesh file format: ", Path);
	LOG_ERROR("Currently only .obj and .gltf formats are supported.");
}

Funky::Asset::RawMesh::RawMesh()
	: IAsset("dynamic asset", Asset::EType::Mesh)
{

}
