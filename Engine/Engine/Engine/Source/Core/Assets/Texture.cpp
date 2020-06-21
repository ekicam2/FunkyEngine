#include "Texture.h"

#include "3rd/stb_image.h"

Funky::Asset::Texture::~Texture()
{
	stbi_image_free(Data);
}

Funky::Asset::Texture* Funky::Asset::Texture::Create(Str const& path)
{
	auto size = Math::Vec2i::Zero;
	i32 channelsFound = 0;
	byte* data = stbi_load(path.GetBuffer(), &(size.X), &(size.Y), &channelsFound, 0);

	auto ret = new Texture();
	if(ret != nullptr && data != nullptr)
	{
		ret->Size = { static_cast<u32>(size.X), static_cast<u32>(size.Y) };
		ret->Path = path;
		ret->Data = data;
		return ret;
	}

	return nullptr;
}
