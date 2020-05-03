#pragma once

#include "BasicTypes.h"

#include "DebugMacros.h"

#include "RenderingBackend/RenderingBackend.h"

#include <d3dcommon.h>
#include <d3d11.h>

#include <vector>

namespace Funky
{
	namespace DirectUtils
	{
		FORCEINLINE charx const * FeatureNameToString(D3D_FEATURE_LEVEL const & fl) {
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
			default: DEAD_PATH();
			}
			return TEXT("unknown");
		}

		FORCEINLINE ::D3D11_BIND_FLAG BufferToDXType(Rendering::RBuffer::EType BufferType)
		{
			using namespace Rendering;

			switch (BufferType)
			{
			case RBuffer::EType::Vertex:  return D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			case RBuffer::EType::Index:   return D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			case RBuffer::EType::Uniform: return D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			case RBuffer::EType::None:    DEAD_PATH(); break;
			}

			return D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		}

		FORCEINLINE D3D11_USAGE BufferToDXUsage(Rendering::RBuffer::EUsageType BufferUsage)
		{
			using namespace Rendering;

			switch (BufferUsage)
			{
			case RBuffer::EUsageType::Default:  return D3D11_USAGE::D3D11_USAGE_DEFAULT;
			case RBuffer::EUsageType::Static:   return D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			case RBuffer::EUsageType::Dynamic:  return D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			case RBuffer::EUsageType::Staging:  return D3D11_USAGE::D3D11_USAGE_STAGING;
			}

			return D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
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
				DEAD_PATH();
				break;
			}
		}
	}
}