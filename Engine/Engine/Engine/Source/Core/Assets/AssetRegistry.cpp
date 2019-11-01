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
	case Funky::Asset::Type::Material:
		AssetPtr = new Asset::Material(Path);
		break;
	}
}

Funky::AssetRegistry::AssetRegistry()
#ifdef FUNKY_EDITOR
	: IWindow("Asset Registry")
#endif // FUNKY_EDITOR
{
	ParseFileTree();
}

#ifdef FUNKY_EDITOR
void Funky::AssetRegistry::DrawGUI()
{
	ImGui::Begin(GetWindowName().c_str());

	// browser
	{
		if (ImGui::Button("Import New Asset") && !Browser.IsOpen)
		{
			Browser.IsOpen = true;
		}

		if (Browser.IsOpen)
		{
			Browser.ResetIfNeeded(BaseAssetsPath);
			Browser.DrawGUI();
		}
	}

	ImGui::Columns(4, "mycolumns");
	ImGui::Separator();
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Path"); ImGui::NextColumn();
	ImGui::Text("Type"); ImGui::NextColumn();
	ImGui::Text("Loaded"); ImGui::NextColumn();
	ImGui::Separator();

	for (auto& Asset : AllAssets)
	{
		if (ImGui::Selectable(Asset.Name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
		{
		}
		ImGui::NextColumn();
		//ImGui::Text(Asset.second->GetPath().c_str()); ImGui::NextColumn();
		ImGui::Text(Asset.Path.c_str()); ImGui::NextColumn();
		ImGui::Text(Asset::TypeToString(Asset.Type).c_str()); ImGui::NextColumn();
		ImGui::Text(Asset.IsLoaded() ? "Loaded" : "Not loaded"); ImGui::NextColumn();
	}
	ImGui::End();
}
#endif // FUNKY_EDITOR


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
				ASSERT(Desc.Type != Asset::Type::Unknown, TEXT("Found unexpected file"));
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
		LOG_ERROR_FUNKY(TEXT("Couldn't open asset file: "), Path);
		BREAK();
		return { "", "", Asset::Type::Unknown };
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

Funky::Asset::IAsset* Funky::AssetRegistry::GetByName(str const& Name, Asset::Type Type)
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

#ifdef FUNKY_EDITOR
void Funky::AssetRegistry::FileBrowser::ResetIfNeeded(str const& NewPath)
{
	if (!ResetNeeded) return;

	CurrentPath = NewPath;
	ResetNeeded = false;
}

void Funky::AssetRegistry::FileBrowser::UpdatePath(str const& NewPath)
{
	CurrentPath = NewPath;

	i32 FileCounter = 0;
	for (const auto& entry : std::filesystem::directory_iterator(CurrentPath))
	{
		if (entry.is_regular_file())
		{
			++FileCounter;
		}
	}

	SelectedFiles.resize(FileCounter);
}

void Funky::AssetRegistry::FileBrowser::DrawGUI()
{
	ImGui::Begin("Import New Asset");

	i32 FileCounter = 0;

	ImVec2 CurrWindowSize = ImGui::GetWindowSize();
	ImGui::BeginChild("BrowserClickables", ImVec2(CurrWindowSize.x, CurrWindowSize.y - 65.0f), true);
	if (ImGui::Selectable(".."))
	{
		UpdatePath(std::filesystem::path(CurrentPath).parent_path().string().c_str());
	}

	for (const auto& entry : std::filesystem::directory_iterator(CurrentPath))
	{
		if (entry.is_directory())
		{
			if (ImGui::Selectable(entry.path().string().c_str()))
			{
				UpdatePath(entry.path().string());
			}
		}
		else
		{
			if (entry.is_regular_file())
			{
				if (ImGui::Selectable(entry.path().string().c_str(), SelectedFiles[FileCounter]))
				{
					SelectedFiles[FileCounter] = !SelectedFiles[FileCounter];
				}
				++FileCounter;
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("Import") && FileCounter > 0)
	{

	}

	ImGui::SameLine(0.0f, 30.0f);

	if (ImGui::Button("Cancel"))
	{
		ResetNeeded = true;
		IsOpen = false;
		SelectedFiles.resize(0);
	}

	ImGui::End();
}
#endif // FUNKY_EDITOR
