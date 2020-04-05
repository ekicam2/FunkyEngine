#include "Cubemap.h"
#include "LogMacros.h"

Funky::Asset::CubemapTexture* Funky::Asset::CubemapTexture::CreateFromFile([[maybe_unused]] Str (&pFilePath)[6])
{
	return nullptr;
/*
	SDL_Surface* image[6];
	for (u8 i = 0; i < 6; ++i)
	{
		image[i] = IMG_Load(pFilePath[i].c_str());
		if (!image[i])
		{
			LOG_ERROR(IMG_GetError());

			for (int j = 0; j < i; ++j)
			{
				SDL_FreeSurface(image[j]);
			}
			return nullptr;
		}

		LOG(TEXT("Texture successful read: ["), i + 1, TEXT("/6]"), pFilePath[i], TEXT(" width: "), image[i]->w, TEXT(" height: "), image[i]->h);
	}

	CubemapTexture* cubemap = new CubemapTexture(str(), ITexture::TextureType::Tex_6D);
	for (u8 i = 0; i < 6; ++i)
	{
		cubemap->Path.append(pFilePath[i]);
		if (i != 5) cubemap->Path.append(" | ");

		cubemap->m_pImage[i] = image[i];
	}

	cubemap->Size = Math::Vec2u(image[0]->w, image[0]->h);
	cubemap->Data.reserve(6 * image[0]->w * image[0]->h);

	for (int i = 0; i < 6; ++i)
	{
		for (u64 pixIndex = 0; pixIndex < (image[0]->w * image[0]->h * 3); pixIndex += 3)
		{
			Uint32 R = ((Uint8*)(image[i]->pixels))[pixIndex];
			Uint32 G = ((Uint8*)(image[i]->pixels))[pixIndex + 1];
			Uint32 B = ((Uint8*)(image[i]->pixels))[pixIndex + 2];
			Uint32 A = 0;

			R = R << image[i]->format->Rshift;
			G = G << image[i]->format->Gshift;
			B = B << image[i]->format->Bshift;

			Uint32 color = 0;
			color |= R;
			color |= G;
			color |= B;
			color |= A;

			cubemap->Data.push_back(color);
		}
	}

	return cubemap;
*/
}

byte* Funky::Asset::CubemapTexture::GetData() const
{
	return (byte*)&Data[0];
}

Funky::Asset::CubemapTexture::~CubemapTexture()
{
	//for (u8 i = 0; i < 6; ++i)
	//{
	//	SDL_FreeSurface(m_pImage[i]);
	//}
}
