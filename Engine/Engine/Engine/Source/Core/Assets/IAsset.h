#pragma once

#include "BasicTypes.h"

#include "Core/String.h"
#include "Macros.h"

namespace Funky
{
	namespace Asset
	{
		using AssetID = str;
		using AssetPath = str;

		enum class EType : u8
		{
			Unknown,
			Texture,
			Mesh,
			Material
		};

		str const TypeToString(Asset::EType EnumType);
		Asset::EType const StringToType(str const & StrType);

		class IAsset
		{
		public:
			IAsset() = delete;
			IAsset(AssetPath const& Path, Asset::EType Type = EType::Unknown) : Path(Path), Type(Type), Loaded(false) {}

			FORCEINLINE AssetPath GetPath() const;
			FORCEINLINE Asset::EType GetType() const;
			FORCEINLINE bool IsLoaded() const;

		protected:
			AssetPath Path;
			Asset::EType Type;

		private:
			bool Loaded;
			friend class AssetRegistry;
		};

	}

	FORCEINLINE Asset::AssetPath Asset::IAsset::GetPath() const
	{
		return Path;
	}

	FORCEINLINE Asset::EType Asset::IAsset::GetType() const
	{
		return Type;
	}

	FORCEINLINE bool Asset::IAsset::IsLoaded() const
	{
		return Loaded;
	}
}