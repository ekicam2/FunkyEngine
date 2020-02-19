#pragma once
#include "RenderingBackend/RenderingBackend.h"
#include "RenderingBackend/Internal/RenderingBackendImpl.h"

#include <d3dcommon.h>
#include <d3d11.h>
// comptr
#include <wrl/client.h>

#include "Core/Containers.h"

#include "Math/Vector2.h"
#include "DirectUtils.h"

namespace Funky
{
	namespace Rendering
	{
		class StaticMesh;

		struct DX11 final : public RenderingBackendImpl
		{
			virtual bool Init(HWND hwnd) override;
			virtual RenderingBackend::API GetBackendAPI() const override;
			virtual void OnViewportResized(Math::Vec2u const & NewSize) override;

			virtual RenderingBackend::RenderTarget CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/) override;

			virtual RenderingBackend::VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize) override;
			virtual RenderingBackend::PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize) override;

			virtual Rendering::RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::Type BufferType, RBuffer::UsageType Usage, RBuffer::Data_t Data = nullptr) override;

			virtual RenderingBackend::Texture CreateTexture2D(byte const * const Data, Math::Vec2u const & Size) override;
			virtual RenderingBackend::Texture CreateCubemap(byte const * const Data, Math::Vec2u const & Size) override;

			virtual void BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind) override;
			virtual void BindDefaultRenderTarget() override;
			virtual void ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear) override;
			virtual void ClearDepthStencil(float Depth, float Stencil) override;
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) override;

			virtual void UpdateBuffer(RBuffer* ConstantBuffer, RBuffer::Data_t Data) override;
			virtual void BindBuffer(RenderingBackend::ShaderResourceStage Stage, RBuffer* Buffer, u32 StartIndex) override;

			virtual void BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind) override;
			virtual void BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex = 0u) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex = 0u) override;

			virtual void Present() override;

			bool CreateDeviceAndSwapchain(HWND hwnd);
			bool InitSwapchain();
			void FreeSwapchain();


		private:
			HWND WindowHandle;

			friend class DX11Renderer;

			struct DX11GPUTexture
			{
				D3D11_TEXTURE2D_DESC TextureDesc;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	pTexture = nullptr;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			};

			struct DX11GPUVertexShader
			{
				Microsoft::WRL::ComPtr<ID3D11InputLayout>  pInputLayout = nullptr;
				Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;;

				DirectUtils::InputLayout InputLayoutDesc;
			};

			struct DX11GPUPixelShader
			{
				Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
			};


			struct DX11GPURenderTarget
			{
				Microsoft::WRL::ComPtr<ID3D11Texture2D>			 pTexture = nullptr;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   pRenderTargetView = nullptr;
			};

			Microsoft::WRL::ComPtr<ID3D11Device>		pDevice = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain>		pSwapChain = nullptr;

			// Resources data
			Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer = nullptr;

			// Resource Views
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  pBackBufferView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  pDepthStencilView = nullptr;

			darray<DX11GPUVertexShader>	VertexShaders;
			darray<DX11GPUPixelShader>		PixelShaders;

			darray<DX11GPUTexture> Textures;

			darray<DX11GPURenderTarget> RenderTargets;

		};
	}
}