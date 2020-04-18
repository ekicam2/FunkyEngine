#pragma once

#include "RenderingBackend/Marker.h"
#include <d3d11_1.h>

#include <locale>
#include <codecvt>
#include <string>

std::wstring utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

namespace Funky::Rendering
{
	class DX11Marker final : public IGPUMarker
	{
	public:
		DX11Marker(ID3DUserDefinedAnnotation* InpMarkersInterface, Str MarkerName)
			: Name(MarkerName)
			, pMarkersInterface(InpMarkersInterface)
		{ 
		}

		DX11Marker(DX11Marker&& Other)
		{
			Name = Other.Name;
			pMarkersInterface = Other.pMarkersInterface;

			Other.pMarkersInterface = nullptr;
		}

		~DX11Marker()
		{
		}

		virtual void Set() override
		{
			pMarkersInterface->BeginEvent(utf8_decode(Name.GetBuffer()).c_str());
		}

		virtual void Free() override
		{
			if (pMarkersInterface)
				pMarkersInterface->EndEvent();
		}

	private:
		Str Name;
		ID3DUserDefinedAnnotation* pMarkersInterface = nullptr;
	};
}
