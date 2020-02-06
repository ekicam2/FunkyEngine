#include "Texture.h"

Funky::Asset::Texture* Funky::Asset::Texture::CreateFromFile(str const & pFilePath)
{
	SDL_Surface* image = IMG_Load(pFilePath.c_str());
	if (!image)
	{
		LOG_ERROR(TEXT("Couldn't load image file: "), IMG_GetError());
		return nullptr;
	}
	LOG(TEXT("Texture successful read: "), pFilePath, TEXT(" width: "), image->w, TEXT(" height: "), image->h);

	Texture* NewCreatedTexture = new Texture(pFilePath, ITexture::TextureType::Tex_1D);
	NewCreatedTexture->Image = image;
	NewCreatedTexture->Path = pFilePath;

	NewCreatedTexture->Data.reserve(image->w * image->h);

	NewCreatedTexture->Size = Math::Vec2u(image->w, image->h);

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

byte* Funky::Asset::Texture::GetData() const
{
	return (byte*)&Data[0];
}

Funky::Asset::Texture::Texture(str const& Path, TextureType Type) : ITexture(Path, Type)
{

}

Funky::Asset::Texture::~Texture()
{
	SDL_FreeSurface(Image);
}
