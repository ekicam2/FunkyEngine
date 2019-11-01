#pragma once

#include "Core/String.h"
#include "Core/Containers.h"

#include "Core/Assets/IAsset.h"

#include "Editor/WindowManager.h"

#include "Core/Assets/AssetTemplates.h"

namespace Funky
{
	class AssetRegistry
#if FUNKY_EDITOR
		: public Editor::IWindow
#endif
	{
	public:
		inline static const str BaseAssetsPath = "AssetsRegistry\\";
		inline static const str AssetExtension = ".fkasset";

		struct AssetDesc
		{
			str Name;
			str Path;
			Asset::Type Type = Asset::Type::Unknown;
			class Asset::IAsset* AssetPtr = nullptr;

			void Load();
			FORCEINLINE bool IsLoaded() const { return AssetPtr != nullptr; }
		};

		AssetRegistry();

		Asset::IAsset* GetByName(str const& Name, Asset::Type Type);

		template <typename T>
		T* GetByName(str const& Name);

	#ifdef FUNKY_EDITOR
		void DrawGUI();
	#endif // FUNKY_EDITOR

	private:
		/* Lookup for all assets in the @BaseAssetsPath and fill @AllAssets. */
		void ParseFileTree();
		void ParseRecursive(str const& Path);

#ifdef FUNKY_EDITOR
		struct FileBrowser
		{
			str CurrentPath;
			bool IsOpen = false;
			bool ResetNeeded = true;

			darray<bool> SelectedFiles;

			void ResetIfNeeded(str const& NewPath);

			void UpdatePath(str const& NewPath);

			void DrawGUI();
		} Browser;
#endif // FUNKY_EDITOR

		static AssetDesc ParseFile(str const& Path);
		darray<AssetDesc> AllAssets;
	};

	template <typename T>
	T* Funky::AssetRegistry::GetByName(str const& Name)
	{
		return static_cast<T*>(GetByName(Name, Asset::CTypeToType<T>()));
	}

}