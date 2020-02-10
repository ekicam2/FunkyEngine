#pragma once

#include "Core/String.h"
#include "Core/Containers.h"

#include "Core/Assets/IAsset.h"


namespace Funky
{
	class AssetRegistry
	{
	public:
		using AssetID = str;

		inline static const str BaseAssetsPath = "AssetsRegistry\\";
		inline static const str AssetExtension = ".fkasset";

		struct AssetDesc
		{
			str Name;
			str Path;
			Asset::EType Type = Asset::EType::Unknown;
			class Asset::IAsset* AssetPtr = nullptr;

			void Load();
			void Free();
			FORCEINLINE bool IsLoaded() const { return AssetPtr != nullptr; }
		};

		AssetRegistry();

		Asset::IAsset* GetByName(AssetID const& Name, Asset::EType Type);

	private:
		/* Lookup for all assets in the @BaseAssetsPath and fill @AllAssets. */
		void ParseFileTree();
		void ParseRecursive(str const& Path);

		static AssetDesc ParseFile(str const& Path);
		darray<AssetDesc> AllAssets;
	};

}