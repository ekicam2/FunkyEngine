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

		inline static const Str BaseAssetsPath = "RealData\\";
		inline static const Str AssetExtension = ".fkasset";

		struct AssetDesc
		{
			Str Path;
			Asset::EAssetType Type = Asset::EAssetType::Unknown;
			class Asset::IAsset* AssetPtr = nullptr;

			//void ToJson() const
			//{
			//	nlohmann::json assetParsed;
			//	assetParsed["path"] = Path;
			//	assetParsed["type"] = Type;
			//	std::cout << assetParsed;
			//}
			//static AssetDesc FromJson(Str const& InJson)
			//{
			//	auto assetParsed = nlohmann::json::parse(InJson.GetBuffer());

			//	AssetDesc Ret;
			//	Ret.Path = assetParsed["path"];
			//	Ret.Type = assetParsed["type"];
			//	return Ret;
			//}

		};

		AssetRegistry();

	private:
		/* Lookup for all .fkassets in the given Path and fill @AllAssets list. */
		void ParseRecursive(Str const& Path);

		darray<AssetDesc> AllAssets;
	};

}