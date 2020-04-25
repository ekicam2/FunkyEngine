#pragma once

#include "Core/Utils.h"
#include "Core/String.h"
#include "Core/Containers.h"

#include "Asset.h"

#include "Material.h"
#include "Shader.h"
#include "StaticMesh.h"


namespace Funky
{
	class AssetRegistry
	{
	public:
		template<typename T>
		using AssetMap = map<Asset::ID, Core::Memory::UniquePtr<T>>;

		inline static const Str AssetExtension = ".fkasset";

#ifdef _DEBUG
		inline static const Str BaseAssetsPath = "RealData\\";
#else
		inline static const Str BaseAssetsPath = "";
#endif
		inline static const Str MeshAssetsPath		= "Meshes\\";
		inline static const Str MaterialAssetsPath	= "Materials\\";
		inline static const Str ShaderAssetsPath	= "Shaders\\";

		AssetRegistry();

		bool IsMeshLoaded(Asset::ID const& MeshID) { return (MeshAssets.find(MeshID) != MeshAssets.end()); }
		bool LoadMesh(Asset::ID const& MeshID) { return MeshID.v[0] > 0; }

		static AssetRegistry& GetInstance()
		{
			static AssetRegistry AR;
			return AR;
		}

		// Do I need such a simple creator?
		//template <typename T>
		//Asset::ID CreateAsset(Str const& Path)
		//{
		//	const Asset::ID id = HashString(Path);
		//	GetBuffer<T>()[id] = T::CreateFromFile(Path);
		//	return id;
		//}

		template <typename T>
		[[deprecated("Use variadic CreateAsset instead.")]]
		Asset::ID CreateAsset(typename T::Desc const& desc)
		{
			if (auto newAsset = T::CreateFromDesc(desc); newAsset != nullptr)
			{
				const Asset::ID id = desc.GetHash();
				GetBuffer<T>()[id].Reset(newAsset);
				return id;
			}

			return Asset::ID::Zero;
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
	};

}