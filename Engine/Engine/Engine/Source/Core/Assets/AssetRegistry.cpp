#include "AssetRegistry.h"

#include "DebugMacros.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Core/Assets/Material.h"

#include "Templates.h"

Funky::AssetRegistry::AssetRegistry()
{
	ParseRecursive(BaseAssetsPath);
}


void Funky::AssetRegistry::ParseRecursive(Str const& Path)
{
	for (const auto& entry : std::filesystem::directory_iterator(Path.GetBuffer()))
	{
		if (entry.is_directory())
		{
			//std::cout << std::setw(level * 3) << "" << filenameStr << '\n';
			ParseRecursive(entry.path().string().c_str());
		}
		else 
		{
			const Str Extension = entry.path().extension().string().c_str();//  filename().string();
			if (entry.is_regular_file() && Extension == AssetExtension)
			{
				//auto Desc = ParseFile(entry.path().string());
				//ASSERT(Desc.Type != Asset::EType::Unknown, TEXT("Found unexpected file"));
				//AllAssets.push_back(Funky::Move(Desc));
			}
		}
	}
}
