#include "AssetRegistry.h"

#include "DebugMacros.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "3rd/imgui/imgui.h"

#include "Core/Assets/Material.h"

#include "Templates.h"

void Funky::AssetRegistry::AssetDesc::Load()
{
	switch (Type)
	{
	case Funky::Asset::EType::Material:
		AssetPtr = new Asset::Material(Path);
		break;
	}
}

void Funky::AssetRegistry::AssetDesc::Free()
{
	delete AssetPtr;
	AssetPtr = nullptr;
}

Funky::AssetRegistry::AssetRegistry()
{
	ParseFileTree();
}


void Funky::AssetRegistry::ParseFileTree()
{
	ParseRecursive(BaseAssetsPath);
}

void Funky::AssetRegistry::ParseRecursive(str const& Path)
{
	for (const auto& entry : std::filesystem::directory_iterator(Path))
	{
		if (entry.is_directory())
		{
			//std::cout << std::setw(level * 3) << "" << filenameStr << '\n';
			ParseRecursive(entry.path().string());
		}
		else 
		{
			const auto Extension = entry.path().extension().string();//  filename().string();
			if (entry.is_regular_file() && Extension == AssetExtension)
			{
				auto Desc = ParseFile(entry.path().string());
				ASSERT(Desc.Type != Asset::EType::Unknown, TEXT("Found unexpected file"));
				AllAssets.push_back(Funky::Move(Desc));
			}
		}
	}
}

/**
 * fkasset files are defined as follows:
 * firts line contains Name
 * second line contains Type
 * Third line contains RealDataPath
 */
Funky::AssetRegistry::AssetDesc Funky::AssetRegistry::ParseFile(str const& Path)
{
	std::ifstream AssetFile(Path, std::ios::binary);
	if (!AssetFile.is_open())
	{
		LOG_ERROR(TEXT("Couldn't open asset file: "), Path);
		BREAK();
		return { "", "", Asset::EType::Unknown };
	}

	AssetDesc Desc;

	std::getline(AssetFile, Desc.Name);
	str Line;
	std::getline(AssetFile, Line);
	Desc.Type = Asset::StringToType(Line);
	std::getline(AssetFile, Desc.Path);

	AssetFile.close();

	return Desc;
}

Funky::Asset::IAsset* Funky::AssetRegistry::GetByName(AssetID const& Name, Asset::EType Type)
{
	for (auto& Asset : AllAssets)
	{
		if (Asset.Name == Name && Asset.Type == Type)
		{
			if (!Asset.IsLoaded())
				Asset.Load();

			return Asset.AssetPtr;
		}
	}

	return nullptr;
}
