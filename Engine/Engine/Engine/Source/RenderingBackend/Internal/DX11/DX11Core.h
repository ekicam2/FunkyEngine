#pragma once
#include "RenderingBackend/RenderingBackend.h"
#include "RenderingBackend/Internal/RenderingBackendImpl.h"

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3d11_1.h>
// comptr
#include <wrl/client.h>

#include "Core/Containers.h"

#include "Math/Vector2.h"
#include "DirectUtils.h"

namespace Funky
{
	namespace Rendering
	{
		struct DX11 final : public RenderingBackendImpl
		{
			virtual bool Init(RenderingBackend::RenderingBackendInitDesc* InitDesc) override;
			virtual RenderingBackend::API GetBackendAPI() const override;
			virtual void OnViewportResized(Math::Vec2u const & NewSize) override;

			virtual RRenderTarget* CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/) override;
			virtual RDepthStencil* CreateDepthStencil(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/) override;

			virtual RShader* CreateVertexShader(RenderingBackend::ShaderInputDesc* ShaderDesc) override;
			virtual RShader* CreatePixelShader(RenderingBackend::ShaderInputDesc* ShaderDesc) override;

			virtual RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::EType BufferType, RBuffer::EUsageType Usage, RBuffer::Data_t Data = nullptr, size Stride = 0u) override;

			virtual RTexture* CreateTexture2D(byte const * const Data, Math::Vec2u const & Size) override;
			virtual RTexture* CreateCubemap(byte const * const Data, Math::Vec2u const & Size) override;

			virtual void BindRenderTarget(RRenderTarget* RenderTargetToBind, RDepthStencil* DepthStencilToBind) override;

			virtual void ClearRenderTarget(RRenderTarget* RenderTargetToClear, Math::Vec3f const& Color) override;
			virtual void ClearDepthStencil(RDepthStencil* DepthStencilToClear, float Depth, float Stencil, bool bClearDepth, bool bClearStencil) override;
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) override;

			virtual void UpdateBuffer(RBuffer* ConstantBuffer, RBuffer::Data_t Data) override;
			virtual void BindBuffer(RenderingBackend::ShaderResourceStage Stage, RBuffer* Buffer, u32 StartIndex) override;

			virtual void BindVertexShader(RShader* VertexShaderToBind) override;
			virtual void BindPixelShader(RShader* PixelShaderToBind) override;
			
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RTexture* Texture, u32 StartIndex = 0u) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RDepthStencil* Texture, u32 StartIndex = 0u) override;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RRenderTarget* Texture, u32 StartIndex = 0u) override;

			virtual void Draw(RBuffer* VertexBuffer) override;
			virtual void DrawIndexed(RBuffer* VertexBuffer, RBuffer* IndexBuffer) override;

			virtual void Present() override;

			virtual Rendering::IGPUMarker* MarkScope(str MarkerName) override;

			bool CreateDeviceAndSwapchain(HWND hwnd);
			bool InitSwapchain();
			void FreeSwapchain();


		private:
			HWND WindowHandle;
			Math::Vec2u WindowSize = { 0u, 0u };

			Microsoft::WRL::ComPtr<ID3D11Device>		pDevice			= nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext	= nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain>		pSwapChain		= nullptr;

			Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> pDebugMarkersStack = nullptr;
		};
	}
}