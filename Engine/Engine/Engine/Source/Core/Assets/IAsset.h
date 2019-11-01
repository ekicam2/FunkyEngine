#pragma once

#include "BasicTypes.h"

#include "Core/String.h"
#include "Macros.h"

namespace Funky
{
	namespace Asset
	{
		enum class Type : u8
		{
			Unknown,
			Texture,
			Mesh,
			Material
		};

		str const TypeToString(Asset::Type EnumType);
		Asset::Type const StringToType(str const & StrType);

		class IAsset
		{
		public:
			IAsset() = delete;
			IAsset(str const& Path, Asset::Type Type = Type::Unknown) : Path(Path), Type(Type), Loaded(false) {}

			FORCEINLINE str GetPath() const;
			FORCEINLINE Asset::Type GetType() const;
			FORCEINLINE bool IsLoaded() const;

		protected:
			str Path;
			Asset::Type Type;

		private:
			bool Loaded;
			friend class AssetRegistry;
		};

	}

	FORCEINLINE str Asset::IAsset::GetPath() const
	{
		return Path;
	}

	FORCEINLINE Asset::Type Asset::IAsset::GetType() const
	{
		return Type;
	}

	FORCEINLINE bool Asset::IAsset::IsLoaded() const
	{
		return Loaded;
	}
}