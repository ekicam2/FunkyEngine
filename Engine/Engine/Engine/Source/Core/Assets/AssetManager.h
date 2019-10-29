#pragma once

#include "Core/String.h"
#include "Core/Containers.h"

#include "Core/Assets/IAsset.h"

namespace Funky
{
	class AssetManager
	{
	public:
		inline static const str BaseAssetsPath = "Resource\\";

		/* Basically lookup for all assets in the @BaseAssetsPath and fill @AllAssets. */
		void ParseFileTree();

		void DrawGUI();


		map<str, Asset::IAsset*> AllAssets;
	};

}