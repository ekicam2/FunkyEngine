#pragma once

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
			struct GPUMesh : RenderingBackend::Mesh { };
			struct GPUTexture : RenderingBackend::Texture { };
			struct GPUVertexShader : RenderingBackend::VertexShader { };
			struct GPUPixelShader : RenderingBackend::PixelShader { };
			struct GPUConstantBuffer : RenderingBackend::ConstantBuffer { size SizeInBytes = 0u; };
			struct GPURenderTarget : RenderingBackend::RenderTarget { };

			virtual RenderingBackend::API GetBackendAPI() const = 0;
			virtual bool Init(HWND hwnd) = 0;
			virtual void OnViewportResized(Math::Vec2u const & NewSize) = 0;

			virtual RenderingBackend::RenderTarget CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/) = 0;

			virtual RenderingBackend::VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize) = 0;
			virtual RenderingBackend::PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize) = 0;
			virtual RenderingBackend::ConstantBuffer CreateConstantBuffer(size SizeOfConstantBuffer) = 0;

			virtual RenderingBackend::Texture CreateTexture2D(byte const * const Data, Math::Vec2u const & Size) = 0;
			virtual RenderingBackend::Texture CreateCubemap(byte const * const Data, Math::Vec2u const & Size) = 0;

			virtual void BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind) = 0;
			virtual void BindDefaultRenderTarget() = 0;
			virtual void ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear) = 0;
			virtual void ClearDepthStencil(float Depth, float Stencil) = 0;
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) = 0;

			virtual void UpdateConstantBuffer(RenderingBackend::ConstantBuffer ConstantBuffer, RenderingBackend::ConstantBufferData Data) = 0;
			virtual void BindConstantBuffer(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::ConstantBuffer const & Buffers, u32 StartIndex) = 0;

			virtual void BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind) = 0;
			virtual void BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex = 0u) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex = 0u) = 0;

			virtual void Present() = 0;
		};
	}
}