#pragma once

#include "Core/Assets/ITexture.h"
#include "Core/Containers.h"

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"

namespace Funky::Asset 
{
	struct CubemapTexture : public ITexture
	{
		static CubemapTexture* CreateFromFile(str (&pFilePath)[6]);
		virtual byte* GetData() const override;

	private:
		SDL_Surface* m_pImage[6];
		darray<u32> Data;

		CubemapTexture(str const& Path, TextureType Type)
			: ITexture(Path, Type)
		{
		}

		~CubemapTexture();
	};
}

