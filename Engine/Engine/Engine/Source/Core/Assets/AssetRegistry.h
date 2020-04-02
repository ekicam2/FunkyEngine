#pragma once

#include "Core/String.h"
#include "Core/Containers.h"

#include "Core/Assets/IAsset.h"

#include "3rd/json.hpp"
#include <iostream>

namespace Funky
{
	class AssetRegistry
	{
	public:
		using AssetID = i64;

		inline static const str BaseAssetsPath = "RealData\\";
		inline static const str AssetExtension = ".fkasset";

		struct AssetDesc
		{
			str Path;
			Asset::EAssetType Type = Asset::EAssetType::Unknown;
			class Asset::IAsset* AssetPtr = nullptr;

			void Load();
			void Free();
			FORCEINLINE bool IsLoaded() const { return AssetPtr != nullptr; }


			void ToJson() const
			{
				nlohmann::json assetParsed;
				assetParsed["path"] = Path;
				assetParsed["type"] = Type;
				std::cout << assetParsed;
			}
			static AssetDesc FromJson(str const& InJson)
			{
				auto assetParsed = nlohmann::json::parse(InJson);

				AssetDesc Ret;
				Ret.Path = assetParsed["path"];
				Ret.Type = assetParsed["type"];
				return Ret;
			}

		};

		AssetRegistry();

	private:
		/* Lookup for all .fkassets in the given Path and fill @AllAssets list. */
		void ParseRecursive(str const& Path);

		darray<AssetDesc> AllAssets;
	};

}