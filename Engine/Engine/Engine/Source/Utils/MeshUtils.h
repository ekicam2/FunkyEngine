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

		static VertexIndexBuffer LoadOBJFromFile(char const* const pFilePath, bool bReverse);
		
		static VertexIndexBuffer LoadGLTFFromFile(char const* const pFilePath, bool bReverseIndices = false);

		//static VertexIndexBuffer CreateCube(f32 Colors[3], bool CCW = false);
	};
}
