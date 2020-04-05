#pragma once

#include "Core/Assets/ITexture.h"
#include "Core/Containers.h"


namespace Funky::Asset 
{
	struct CubemapTexture : public ITexture
	{
		static CubemapTexture* CreateFromFile(Str (&pFilePath)[6]);
		virtual byte* GetData() const override;

	private:
		darray<u32> Data;

		CubemapTexture(Str const& Path, ETextureType Type)
			: ITexture(Path, Type)
		{
		}

		~CubemapTexture();
	};
}

