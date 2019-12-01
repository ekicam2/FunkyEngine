#include "RawMesh.h"
#include "Utils/MeshUtils.h"
#include "Templates.h"

Funky::Asset::RawMesh::RawMesh(str const& Path)
	: IAsset(Path, Asset::EType::Mesh)
{
	if (!Path.ends_with(".obj"))
	{
		LOG_ERROR_FUNKY("Unsupported mesh file format: ", Path);
		LOG_ERROR("Currently only .obj format is supported.");
		return;
	}

	auto VertsAndInds = Funky::MeshUtils::LoadOBJFromFile(Path.c_str());

	Vertices = Funky::Move(VertsAndInds.first);
	Indices = Funky::Move(VertsAndInds.second);
}

Funky::Asset::RawMesh::RawMesh(std::pair<darray<Vertex>, darray<u16>>&& Load)
	: IAsset(str("CustomMesh").append(std::to_string(RawMesh::TagCounter)), Asset::EType::Mesh)
{
	TagCounter++;

	Vertices = Funky::Move(Load.first);
	Indices = Funky::Move(Load.second);
}
