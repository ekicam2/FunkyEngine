#pragma once

#include "BasicTypes.h"
#include "RenderingBackend/RenderingBackend.h"

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
			virtual RenderingBackend::API GetBackendAPI() const = 0;
			virtual bool Init(HWND hwnd) = 0;
			virtual void OnViewportResized(Math::Vec2u const & NewSize) = 0;

			virtual RenderingBackend::RenderTarget CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/) = 0;

			virtual Rendering::RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::Type BufferType, RBuffer::UsageType Usage, RBuffer::Data_t Data = nullptr) = 0;


			virtual RenderingBackend::VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize) = 0;
			virtual RenderingBackend::PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize) = 0;

			virtual RenderingBackend::Texture CreateTexture2D(byte const * const Data, Math::Vec2u const & Size) = 0;
			virtual RenderingBackend::Texture CreateCubemap(byte const * const Data, Math::Vec2u const & Size) = 0;

			virtual void BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind) = 0;
			virtual void BindDefaultRenderTarget() = 0;
			virtual void ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear) = 0;
			virtual void ClearDepthStencil(float Depth, float Stencil) = 0;
			
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) = 0;

			virtual void UpdateBuffer(RBuffer* const ConstantBuffer, RBuffer::Data_t Data) = 0;
			virtual void BindBuffer(RenderingBackend::ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex) = 0;

			virtual void BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind) = 0;
			virtual void BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex = 0u) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex = 0u) = 0;

			virtual void Present() = 0;
		};
	}
}