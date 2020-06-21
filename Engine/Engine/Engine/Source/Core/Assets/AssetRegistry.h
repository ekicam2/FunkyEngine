#pragma once

#include "Core/Utils.h"
#include "Core/String.h"
#include "Core/Containers.h"

#include "Asset.h"

#include "Material.h"
#include "Shader.h"
#include "StaticMesh.h"
#include "Texture.h"


namespace Funky
{
	class AssetRegistry
	{
	public:
		template<typename T>
		using AssetMap = map<Asset::ID, Core::Memory::UniquePtr<T>>;

#ifdef _DEBUG
		inline static const Str BaseAssetsPath = "RealData\\";
#else
		inline static const Str BaseAssetsPath = "";
#endif
		inline static const Str MeshAssetsPath		= "Meshes\\";
		inline static const Str MaterialAssetsPath	= "Materials\\";
		inline static const Str ShaderAssetsPath    = "Shaders\\";
		inline static const Str TextureAssetPath    = "Texture\\";

		AssetRegistry();

		static AssetRegistry& GetInstance()
		{
			static AssetRegistry AR;
			return AR;
		}

		template<typename T, typename... Args>
		Asset::ID CreateAsset(Args... args)
		{
			if (auto newAsset = T::Create(Forward<Args>(args)...); newAsset != nullptr)
			{
				const Asset::ID id = newAsset->GetHash();
				GetBuffer<T>()[id].Reset(newAsset);
				return id;
			}

			return Asset::ID::Zero;
		}

		template <typename T>
		T* GetAsset(Asset::ID const& assetID)
		{
			auto& map = GetBuffer<T>();
			if (auto found = map.find(assetID); found != map.end())
			{
				return (*found).second.Get();
			}

			return nullptr;
		}

	protected:
		template<typename T>
		AssetMap<T>& GetBuffer() 
		{ 
			static_assert(false, "Implement GetBuffer() method specialization for yout new type.");
		}

		template<>
		AssetMap<Asset::StaticMesh>& GetBuffer() { return MeshAssets; }
		template<>
		AssetMap<Asset::Material>& GetBuffer() { return MaterialAssets; }
		template<>
		AssetMap<Asset::Shader>& GetBuffer() { return ShadersAssets; }
	private:
		AssetMap<Asset::StaticMesh> MeshAssets;
		AssetMap<Asset::Material>	MaterialAssets;
		AssetMap<Asset::Shader>		ShadersAssets;
		AssetMap<Asset::Texture>	TexturesAssets;
	};

}