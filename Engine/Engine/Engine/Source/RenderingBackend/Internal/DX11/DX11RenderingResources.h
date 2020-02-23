#pragma once

#include "RenderingFrontend/IRenderingResource.h"

#include <RenderingBackend/Internal/DX11/DX11Core.h>

//#define FUNKY_D3D11_SAFE_RELEASE(x)		\
//	if(x)								\
//	{									\
//		x->Release();					\
//		x = nullptr;					\
//	}

#define FUNKY_D3D11_SAFE_RELEASE(x)	


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

			~DX11RenderTarget()
			{
				FUNKY_D3D11_SAFE_RELEASE(pTextureView);
				FUNKY_D3D11_SAFE_RELEASE(pRenderTargetView);
				FUNKY_D3D11_SAFE_RELEASE(pTexture);
			}
		};

		class DX11DepthStencil final : public RDepthStencil
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			 pTexture = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   pDepthStencilView = nullptr;

			~DX11DepthStencil()
			{
				FUNKY_D3D11_SAFE_RELEASE(pTexture);
				FUNKY_D3D11_SAFE_RELEASE(pTextureView);
				FUNKY_D3D11_SAFE_RELEASE(pDepthStencilView);
			}
		};

		class DX11Buffer final : public RBuffer
		{
		public:
			DX11Buffer(Type NewType, UsageType NewUsageType) : RBuffer(NewType, NewUsageType) {}
			Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = nullptr;
			size SizeInBytes = 0u;
			u32 Stride = 0u;
			u32 Offset = 0u;

			~DX11Buffer()
			{
				FUNKY_D3D11_SAFE_RELEASE(pBuffer);
			}
		};

		class DX11VertexShader : public RShader
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> pVs;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
			
			DX11VertexShader()
			{
				ShaderType = (RShader::Type::Vertex);
			}

			~DX11VertexShader()
			{
				FUNKY_D3D11_SAFE_RELEASE(pVs);
				FUNKY_D3D11_SAFE_RELEASE(pInputLayout);
			}
		};

		class DX11PixelShader : public RShader
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pPs;
			
			~DX11PixelShader()
			{
				FUNKY_D3D11_SAFE_RELEASE(pPs);
			}
		};

		class DX11Texture : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			
			~DX11Texture()
			{
				FUNKY_D3D11_SAFE_RELEASE(pTextureView);
			}
		};

		class DX11Texture2D final : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>	pTexture = nullptr;

			~DX11Texture2D()
			{
				FUNKY_D3D11_SAFE_RELEASE(pTexture);
			}
		};

		class DX11Texture3D final : public RTexture
		{
		public:
			Microsoft::WRL::ComPtr<ID3D11Texture3D>	pTexture = nullptr;

			~DX11Texture3D()
			{
				FUNKY_D3D11_SAFE_RELEASE(pTexture);
			}
		};
	}
}