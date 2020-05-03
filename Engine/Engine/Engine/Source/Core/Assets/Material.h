#pragma once

#include "Asset.h"

#define UBS_PATH "RealData/Shaders/Source/ubershader.hlsl"
#define TOON_PATH UBS_PATH
#define DL_PATH UBS_PATH
#define UNLIT_PATH UBS_PATH

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
				Ubershader, // debug

				Toon,
				DefaultLit,
				Unlit,
				Custom
			} Technique;
			const static Str MaterialTechniqueToString[];
			const static Str MaterialTechniqueToPath[];

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

			FORCEINLINE Hash128 GetHash() const { return HashString(Path + MaterialTechniqueToString[static_cast<u8>(Technique)]); }
		private:
			Str Path;

			[[nodiscard]]
			static Material* CreateFromDesc(Desc const& desc);

			[[nodiscard]]
			static Material* Create(ERenderingTechnique technique);

			[[nodiscard]]
			static Material* Create(Str const& path, ERenderingTechnique technique = ERenderingTechnique::Custom);
			[[nodiscard]]
			static Material* Create(Str const& path_vs, Str const & path_ps, ERenderingTechnique technique = ERenderingTechnique::Custom);
		};
		inline const Str Material::MaterialTechniqueToString[] = { "UberShader", "Toon", "DefaultLit", "Unlit", "Custom"};
		inline const Str Material::MaterialTechniqueToPath[] = { UBS_PATH, TOON_PATH, DL_PATH, UNLIT_PATH, "\0"};

	}
}
