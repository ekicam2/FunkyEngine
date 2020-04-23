#pragma once

#include "BasicTypes.h"
#include "RenderingBackend/RenderingBackend.h"
#include "RenderingFrontend/RenderingResourceManager.h"

#ifndef WINDOWS_LEAN_AND_MEAN
	#define WINDOWS_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Funky
{
	namespace Rendering
	{
		class RenderingBackendImpl
		{
		public:
			virtual ~RenderingBackendImpl() {}
			virtual RenderingBackend::EAPI GetBackendAPI() const = 0;
			virtual bool Init(RenderingBackend::RenderingBackendInitDesc* InitDesc, RenderingBackend::RenderingBackendInitResult* Result) = 0;
			virtual void OnViewportResized(Math::Vec2u const & NewSize) = 0;

			virtual RRenderTarget* CreateRenderTarget(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/) = 0;
			virtual RDepthStencil* CreateDepthStencil(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/) = 0;

			virtual RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::EType BufferType, RBuffer::EUsageType Usage, RBuffer::Data_t Data = nullptr, size Stride = 0u) = 0;

			virtual RShader* CreateVertexShader(RenderingBackend::ShaderInputDesc* ShaderDesc) = 0;
			virtual RShader* CreatePixelShader(RenderingBackend::ShaderInputDesc* ShaderDesc) = 0;

			virtual RTexture* CreateTexture2D(byte const * const Data, Math::Vec2u const & Size) = 0;
			virtual RTexture* CreateCubemap(byte const * const Data, Math::Vec2u const & Size) = 0;

			virtual void BindRenderTarget(RRenderTarget* RenderTargetToBind, RDepthStencil* DepthStencilToBind) = 0;

			virtual void ClearRenderTarget(RRenderTarget* RenderTargetToClear, Math::Vec3f const& Color) = 0;
			virtual void ClearDepthStencil(RDepthStencil* DepthStencilToClear, float Depth, float Stencil, bool bClearDepth, bool bClearStencil) = 0;
			
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) = 0;

			virtual void UpdateBuffer(RBuffer* const ConstantBuffer, RBuffer::Data_t Data) = 0;
			virtual void BindBuffer(RenderingBackend::ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex) = 0;

			virtual void BindVertexShader(RShader* VertexShaderToBind) = 0;
			virtual void BindPixelShader(RShader* PixelShaderToBind) = 0;

			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RTexture* Texture, u32 StartIndex = 0u) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RDepthStencil* Texture, u32 StartIndex = 0u) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RRenderTarget* Texture, u32 StartIndex = 0u) = 0;

			virtual void Draw(RBuffer* VertexBuffer) = 0;
			virtual void DrawIndexed(RBuffer* VertexBuffer, RBuffer* IndexBuffer) = 0;

			virtual void Present() = 0;

			virtual Rendering::IGPUMarker* MarkScope(Str MarkerName) = 0;
		};}
}