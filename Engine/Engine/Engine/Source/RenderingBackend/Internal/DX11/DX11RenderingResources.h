#pragma once

#include "RenderingFrontend/IRenderingResource.h"

#include <RenderingBackend/Internal/DX11/DX11Core.h>

namespace Funky
{
	namespace Rendering
	{
		class DX11RenderTarget final : public RRenderTarget
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			 pTexture = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   pRenderTargetView = nullptr;
		};

		class DX11DepthStencil final : public RDepthStencil
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			 pTexture = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   pDepthStencilView = nullptr;
		};

		class DX11Buffer final : public RBuffer
		{
		public:
			DX11Buffer(EType NewType, EUsageType NewUsageType) { BufferType = NewType; Usage = NewUsageType; }
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = nullptr;
			size SizeInBytes = 0u;
			u32 Stride = 0u;
			u32 Offset = 0u;
		};

		class DX11VertexShader : public RShader
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> pVs;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
			
			DX11VertexShader();
		};

		class DX11PixelShader : public RShader
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pPs;
			
			DX11PixelShader();
		};

		class DX11Texture : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
		};

		class DX11Texture2D final : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>	pTexture = nullptr;
		};

		class DX11Texture3D final : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture3D>	pTexture = nullptr;
		};
	}
}