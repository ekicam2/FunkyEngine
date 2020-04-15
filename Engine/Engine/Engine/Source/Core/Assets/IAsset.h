#pragma once

#include "BasicTypes.h"

#include "Core/String.h"
#include "Macros.h"

namespace Funky
{
	namespace Asset
	{
		using AssetPath = Str;

		enum class EAssetType : u8
		{
			Unknown,
			Texture,
			Mesh,
			Shader,
			Material,

			Count
		};

		Str const TypeToString(Asset::EAssetType EnumType);
		Asset::EAssetType StringToType(Str const & StrType);

		class IAsset
		{
		public:
			IAsset(EAssetType InType) : Type(InType) {}
			
			virtual ~IAsset() = default;

			
			FORCEINLINE AssetPath GetPath() const;
			FORCEINLINE Asset::EAssetType GetType() const;
			FORCEINLINE bool IsLoaded() const;

		private:
			AssetPath Path;
			Asset::EAssetType Type = EAssetType::Unknown;

			bool Loaded = false;
		};
	}

	FORCEINLINE Asset::AssetPath Asset::IAsset::GetPath() const
	{
		return Path;
	}

	FORCEINLINE Asset::EAssetType Asset::IAsset::GetType() const
	{
		return Type;
	}

	FORCEINLINE bool Asset::IAsset::IsLoaded() const
	{
		return Loaded;
	}
}

// use in the private space
#define DECLARE_IASSET(Name, Type)	\
	Name() : IAsset(Type) {}		
