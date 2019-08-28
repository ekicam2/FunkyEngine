#pragma once

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"
#include "RenderingBackend.h"

#include "Core/Containers.h"
#include "Core/String.h"

namespace Funky
{
	struct TextureBase
	{
		str FilePath;
		Math::Vector2u Size = Math::Vector2u(0u, 0u);
		Rendering::RenderingBackend::Texture Proxy = Rendering::RenderingBackend::INVALID_INDEX;
		
		virtual byte* GetData() const = 0;
		bool HasValidProxy() const { return Proxy == Rendering::RenderingBackend::INVALID_INDEX; }
	};

	struct Texture2D : TextureBase
	{
		static Texture2D* CreateFromFile(const char* pFilePath)
		{
			SDL_Surface* image = IMG_Load(pFilePath);
			if (!image)
			{
				LOG_ERROR_FUNKY(TEXT("Couldn't load image file: "), IMG_GetError());
				return nullptr;
			}
			LOG_FUNKY(TEXT("Texture successful read: "), pFilePath, TEXT(" width: "), image->w, TEXT(" height: "), image->h);

			Texture2D* NewCreatedTexture = new Texture2D();
			NewCreatedTexture->Image = image;
			NewCreatedTexture->FilePath = pFilePath;

			NewCreatedTexture->Data.reserve(image->w * image->h);

			NewCreatedTexture->Size = Math::Vector2u(image->w, image->h);

			for (u64 pixIndex = 0; pixIndex < (image->w * image->h * 3); pixIndex += 3)
			{
				Uint32 R = ((Uint8*)(image->pixels))[pixIndex];
				Uint32 G = ((Uint8*)(image->pixels))[pixIndex + 1];
				Uint32 B = ((Uint8*)(image->pixels))[pixIndex + 2];
				Uint32 A = 0;

				R = R << image->format->Rshift;
				G = G << image->format->Gshift;
				B = B << image->format->Bshift;

				Uint32 color = 0;
				color |= R;
				color |= G;
				color |= B;
				color |= A;

				NewCreatedTexture->Data.push_back(color);
			}

			return NewCreatedTexture;
		}

		virtual byte* GetData() const override
		{
			return (byte*)&Data[0];
		}

	private:
		SDL_Surface* Image;
		darray<u32> Data;

		Texture2D() = default;
		~Texture2D()
		{
			SDL_FreeSurface(Image);
		}
	};

	struct CubemapTexture : TextureBase
	{
		static CubemapTexture* CreateFromFile(const char* pFilePath[6])
		{
			SDL_Surface* image[6];
			for (u8 i = 0; i < 6; ++i)
			{
				image[i] = IMG_Load(pFilePath[i]);
				if (!image[i])
				{
					LOG_ERROR_FUNKY(IMG_GetError());

					for (int j = 0; j < i; ++j)
					{
						SDL_FreeSurface(image[j]);
					}
					return nullptr;
				}

				LOG_FUNKY(TEXT("Texture successful read: ["), i + 1, TEXT("/6]"), pFilePath[i], TEXT(" width: "), image[i]->w, TEXT(" height: "), image[i]->h);
			}

			CubemapTexture* cubemap = new CubemapTexture();
			for (u8 i = 0; i < 6; ++i)
			{
				cubemap->FilePath.append(pFilePath[i]);
				if(i != 5) cubemap->FilePath.append(" | ");

				cubemap->m_pImage[i] = image[i];
			}

			cubemap->Size = Math::Vector2u(image[0]->w, image[0]->h);
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
		}

		virtual byte* GetData() const override
		{
			return (byte*)&Data[0];
		}

	private:
		SDL_Surface* m_pImage[6];
		darray<u32> Data;

		CubemapTexture() = default;
		~CubemapTexture()
		{
			for (u8 i = 0; i < 6; ++i)
			{
				SDL_FreeSurface(m_pImage[i]);
			}
		}
	};
}