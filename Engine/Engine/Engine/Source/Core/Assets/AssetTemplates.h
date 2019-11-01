#pragma once

#include "Core/Assets/IAsset.h"

namespace Funky
{
	namespace Asset
	{
		struct Material;

		template <typename T>
		inline Asset::Type CTypeToType() { return Asset::Type::Unknown; }

		template <>
		inline Asset::Type CTypeToType<Asset::Material>() { return Asset::Type::Material; }

	}
}

