#pragma once

#include "Asset.h"

namespace Funky
{
	class AssetRegistry;

	namespace Asset
	{
		struct Material
		{
			friend class Funky::AssetRegistry;

			enum class ERenderingTechnique : u8
			{
				Toon,
				DefaultLit,
				Unlit
			} Technique;
			const static Str MaterialTechniqueToString[];

			struct Desc
			{
				/** Required */
				FORCEINLINE Hash128 GetHash() const { return HashString(Name + MaterialTechniqueToString[static_cast<u8>(Technique)]); }
				/** Required END */

				Str Name;
				ERenderingTechnique Technique;

				Asset::ID PS;
				Asset::ID VS;
			};

			Asset::ID PS;
			Asset::ID VS;

		private:
			[[nodiscard]]
			static Material* CreateFromDesc(Desc const& desc);
		};
		inline const Str Material::MaterialTechniqueToString[] = { "Toon", "DefaultLit", "Unlit" };

	}
}
