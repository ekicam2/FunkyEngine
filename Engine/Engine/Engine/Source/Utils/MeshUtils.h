#pragma once

#include "Core/Assets/RawMesh.h"
#include "Core/Containers.h"

namespace Funky
{
	class MeshUtils
	{
	public: 

		static MeshLoad CreateTerrainPlane(u32 GridCellsX, u32 GridCellsY, f32 StepBetweenGridPoints = 0.0f);

		static MeshLoad CreateCube();
		
		static MeshLoad CreateCube(f32 Colors[3], bool CCW = false);
		
		static MeshLoad LoadOBJFromFile(char const * const pFilePath);
		
		static MeshLoad CreateSphere(f32 Radius, bool CCW = false);
	};
}