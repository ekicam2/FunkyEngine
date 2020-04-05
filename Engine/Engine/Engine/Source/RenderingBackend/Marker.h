#pragma once

#include "RenderingBackend/RenderingBackend.h"

#include "BasicTypes.h"
#include "Core/String.h"

namespace Funky::Rendering
{
	class IGPUMarker
	{
	public:
		virtual void Set() = 0;
		virtual void Free() = 0;
	};

	class GPUScopeMarker
	{
	public:
		GPUScopeMarker(Rendering::RenderingBackend* pRenderingBackend, Str MarkerName)
		{
			Marker.Reset(pRenderingBackend->MarkScope(MarkerName));
			Marker->Set();
		}

		~GPUScopeMarker()
		{
			Marker->Free();
		}

	protected:
		Core::Memory::UniquePtr<IGPUMarker> Marker;
	};
}
