#pragma once

#define SDL_MAIN_HANDLED
#include "3rd/SDL_image.h"
#include "Rendering/RenderingBackend.h"

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"

#include "DebugMacros.h"

namespace Funky
{
	namespace Asset
	{
		struct ITexture : public IAsset
		{
			enum class TextureType
			{
				UNKNOWN, // !< Not supported
				Tex_1D,
				Tex_6D
			} Type;

			ITexture(str const& Path, TextureType Type);


			Math::Vector2u Size = Math::Vector2u(0u, 0u);
			Rendering::RenderingBackend::Texture Proxy = Rendering::RenderingBackend::INVALID_INDEX;
		
			virtual byte* GetData() const = 0;
			FORCEINLINE bool HasValidProxy() const { return Proxy != Rendering::RenderingBackend::INVALID_INDEX; }
		};

	}
}