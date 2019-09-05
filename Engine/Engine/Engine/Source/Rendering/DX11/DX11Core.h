#pragma once
#include "Rendering/RenderingBackend.h"
#include "Rendering/RenderingBackendImpl.h"

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
		class RawMesh;

		struct DX11 final : public RenderingBackendImpl
		{
			virtual bool Init(HWND hwnd) override;
			virtual RenderingBackend::API GetBackendAPI() const override;
			virtual void OnViewportResized(Math::Vector2u const & NewSize) override;

			virtual RenderingBackend::RenderTarget CreateRenderTarget(Math::Vector2u const & Size /* TODO(ekicam2): I woild like to specify format*/) override;

			virtual RenderingBackend::VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize) override;
			virtual RenderingBackend::PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize) override;
			virtual RenderingBackend::ConstantBuffer CreateConstantBuffer(size SizeOfConstantBuffer) override;

			virtual RenderingBackend::Texture CreateTexture2D(byte const * const Data, Math::Vector2u const & Size) override;
			virtual RenderingBackend::Texture CreateCubemap(byte const * const Data, Math::Vector2u const & Size) override;

			virtual RenderingBackend::MeshProxy CreateMeshProxy(Funky::RawMesh const * Mesh) override;

			virtual void BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind) override;
			virtual void BindDefaultRenderTarget() override;
			virtual void ClearRenderTargetWithColor(Math::Vector3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear) override;
			virtual void ClearDepthStencil(float Depth, float Stencil) override;
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) override;

			virtual void UpdateConstantBuffer(RenderingBackend::ConstantBuffer ConstantBuffer, RenderingBackend::ConstantBufferData Data) override;
			virtual void BindConstantBuffer(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::ConstantBuffer const & Buffers, u32 StartIndex) override;

			virtual void BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind) override;
			virtual void BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex = 0u) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex = 0u) override;
			virtual void DrawMesh(RenderingBackend::MeshProxy Mesh) override;

			virtual void Present() override;

			bool CreateDeviceAndSwapchain(HWND hwnd);
			bool InitSwapchain();
			void FreeSwapchain();


		private:
			HWND WindowHandle;

			friend class DX11Renderer;

			struct DX11GPUMesh : GPUMesh
			{
				Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
				Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;

				u64 IndicesCount = 0;
			};

			struct DX11GPUTexture : GPUTexture
			{
				D3D11_TEXTURE2D_DESC TextureDesc;

				Microsoft::WRL::ComPtr<ID3D11Texture2D>	pTexture = nullptr;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			};

			struct DX11GPUVertexShader : GPUVertexShader
			{
				Microsoft::WRL::ComPtr<ID3D11InputLayout>  pInputLayout = nullptr;
				Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;;

				DirectUtils::InputLayout InputLayoutDesc;
			};

			struct DX11GPUPixelShader : GPUPixelShader
			{
				Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
			};

			struct DX11GPUConstantBuffer : GPUConstantBuffer
			{
				Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
			};

			struct DX11GPURenderTarget : GPURenderTarget
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
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  pRenderTarget = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  pDepthStencilView = nullptr;

			darray<DX11GPUConstantBuffer>	ConstantBuffers;
			darray<DX11GPUVertexShader>	VertexShaders;
			darray<DX11GPUPixelShader>		PixelShaders;

			darray<DX11GPUTexture> Textures;

			darray<DX11GPUMesh> Meshes;

			darray<DX11GPURenderTarget> RenderTargets;

		};
	}
}