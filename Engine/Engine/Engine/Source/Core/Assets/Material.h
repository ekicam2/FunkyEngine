#pragma once

#include "Core/Containers.h"
#include "Core/String.h"

#include "Core/Assets/IAsset.h"

#include <optional>
#include <fstream>

#include "Macros.h"
#include "Rendering/RenderingBackend.h"

namespace Funky
{
	namespace Asset
	{
		struct Material : public IAsset
		{
			enum class RenderingTechnique
			{
				DefaultLit,
				Internal_Depth
			};

			explicit Material(str const & Path);
			FORCEINLINE bool IsValid();
			static std::optional<std::pair<str, str>> ParseMaterial(str const& Path);

			Rendering::RenderingBackend::VertexShader VS = Rendering::RenderingBackend::INVALID_INDEX;
			Rendering::RenderingBackend::PixelShader PS = Rendering::RenderingBackend::INVALID_INDEX;
			RenderingTechnique Technique = RenderingTechnique::DefaultLit;
		};
	}
}
