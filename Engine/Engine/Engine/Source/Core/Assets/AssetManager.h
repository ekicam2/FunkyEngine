#pragma once

#include "Core/String.h"
#include "Core/Containers.h"

#include "Core/Assets/IAsset.h"

#include "Editor/WindowManager.h"

namespace Funky
{
	class AssetManager
#if FUNKY_EDITOR
		: public Editor::IWindow
#endif
	{
	public:
		inline static const str BaseAssetsPath = "Resource\\";

		AssetManager();
		~AssetManager();

		void DrawGUI();

	private:
		/* Lookup for all assets in the @BaseAssetsPath and fill @AllAssets. */
		void ParseFileTree();

		void ParseRecursive(str const& Path);
		void ParseAsset(str const& Path);

		map<str, Asset::IAsset*> AllAssets;
	};

}