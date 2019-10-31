#include "AssetManager.h"

#include "DebugMacros.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "3rd/imgui/imgui.h"

void Funky::AssetManager::ParseFileTree()
{
	ParseRecursive(BaseAssetsPath);
}

void Funky::AssetManager::DrawGUI()
{
	ImGui::Begin(GetWindowName().c_str());
	for (auto& Asset : AllAssets)
	{
		if (Asset.second != nullptr)
		{
			ImGui::Text(Asset.second->GetPath().c_str());
			ImGui::SameLine(0.0f, 45.0f);
			ImGui::Text(Asset.second->IsLoaded() ? "Loaded" : "Not loaded");
		}
	}
	ImGui::End();
}

Funky::AssetManager::AssetManager()
#ifdef FUNKY_EDITOR
	: IWindow("AssetManager")
#endif // FUNKY_EDITOR
{
	ParseFileTree();
}

void Funky::AssetManager::ParseRecursive(str const& Path)
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
			if (entry.is_regular_file() && Extension == ".fkasset")
			{
				ParseAsset(entry.path().string());
			}
			//std::cout << std::setw(level * 3) << "" << filenameStr << ", size " << entry.file_size() << " bytes\n";
		}
	}
}

void Funky::AssetManager::ParseAsset(str const& Path)
{
	std::ifstream AssetFile(Path, std::ios::binary);
	if (!AssetFile.is_open())
	{
		LOG_ERROR_FUNKY(TEXT("Couldn't parse asset: "), Path);
		BREAK();
		return;
	}

	str Line;
	while (std::getline(AssetFile, Line))
	{
		LOG(Line);
	}

	auto Temp = new Asset::IAsset(Path);
	AllAssets[":tetet"] = Temp;

	AssetFile.close();
}

Funky::AssetManager::~AssetManager()
{
	for (auto& Asset : AllAssets)
	{
		delete Asset.second;
	}
}
