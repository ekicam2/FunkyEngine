#pragma once

#include "BasicTypes.h"

#include "DebugMacros.h"

#include <d3dcommon.h>
#include <d3d11.h>

#include <vector>

namespace Funky
{
	namespace DirectUtils
	{
		FORCEINLINE charx const * const FeatureNameToString(D3D_FEATURE_LEVEL const & fl) {
			switch (fl) {
			case D3D_FEATURE_LEVEL_9_1:  return TEXT("D3D_FEATURE_LEVEL_9_1");
			case D3D_FEATURE_LEVEL_9_2:  return TEXT("D3D_FEATURE_LEVEL_9_2");
			case D3D_FEATURE_LEVEL_9_3:  return TEXT("D3D_FEATURE_LEVEL_9_3");
			case D3D_FEATURE_LEVEL_10_0: return TEXT("D3D_FEATURE_LEVEL_10_0");
			case D3D_FEATURE_LEVEL_10_1: return TEXT("D3D_FEATURE_LEVEL_10_1");
			case D3D_FEATURE_LEVEL_11_0: return TEXT("D3D_FEATURE_LEVEL_11_0");
			case D3D_FEATURE_LEVEL_11_1: return TEXT("D3D_FEATURE_LEVEL_11_1");
			case D3D_FEATURE_LEVEL_12_0: return TEXT("D3D_FEATURE_LEVEL_12_0");
			case D3D_FEATURE_LEVEL_12_1: return TEXT("D3D_FEATURE_LEVEL_12_1");
			default: DEATH_PATH();
			}
			return TEXT("unknown");
		}

		// atm it's enough
		enum class InputLayout {
			PositionOnly,
		
			PositionColor,
			PositionColorUV,
			PositionColorNormalUV,
		};

		FORCEINLINE void GetInputLayoutDesc(InputLayout eInputLayout, std::vector<D3D11_INPUT_ELEMENT_DESC>& outDesc) {
			switch (eInputLayout)
			{
			/*case DirectUtils::InputLayout::PositionOnly:
				outDesc = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				break;
			case DirectUtils::InputLayout::PositionColor:
				outDesc = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				break;*/
			case DirectUtils::InputLayout::PositionColorNormalUV:
				outDesc = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				break;

			//case DirectUtils::InputLayout::PositionColorUV:
			//	outDesc = {
			//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//	};
			//	break;
			default:
				DEATH_PATH();
				break;
			}
		}
	}
}