#pragma once

#include "RenderingFrontend/IRenderingResource.h"

#include <RenderingBackend/Internal/DX11/DX11Core.h>

namespace Funky
{
	namespace Rendering
	{
		class DX11Buffer final : public RBuffer
		{
		public:
			DX11Buffer(Type NewType, UsageType NewUsageType) : RBuffer(NewType, NewUsageType) {}
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = nullptr;
			size SizeInBytes = 0u;
		};
	}
}