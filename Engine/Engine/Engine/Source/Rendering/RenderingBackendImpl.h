#pragma once

#include "RenderingBackend.h"

#ifndef WINDOWS_LEAN_AND_MEAN
	#define WINDOWS_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "RawMesh.h"

namespace Funky
{
	namespace Rendering
	{
		class RenderingBackendImpl
		{
		public:
			struct GPUMesh { };
			struct GPUTexture { };
			struct GPUVertexShader { };
			struct GPUPixelShader { };
			struct GPUConstantBuffer { size SizeInBytes = 0u; };
			struct GPURenderTarget { };

			virtual RenderingBackend::API GetBackendAPI() const = 0;
			virtual bool Init(HWND hwnd) = 0;
			virtual void OnViewportResized(Math::Vector2u const & NewSize) = 0;

			virtual RenderingBackend::RenderTarget CreateRenderTarget(Math::Vector2u const & Size /* TODO(ekicam2): I woild like to specify format*/) = 0;

			virtual RenderingBackend::VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize) = 0;
			virtual RenderingBackend::PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize) = 0;
			virtual RenderingBackend::ConstantBuffer CreateConstantBuffer(size SizeOfConstantBuffer) = 0;

			virtual RenderingBackend::Texture CreateTexture2D(byte const * const Data, Math::Vector2u const & Size) = 0;
			virtual RenderingBackend::Texture CreateCubemap(byte const * const Data, Math::Vector2u const & Size) = 0;

			virtual RenderingBackend::MeshProxy CreateMeshProxy(Funky::RawMesh const * Mesh) = 0;

			virtual void BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind) = 0;
			virtual void BindDefaultRenderTarget() = 0;
			virtual void ClearRenderTargetWithColor(Math::Vector3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear) = 0;
			virtual void ClearDepthStencil(float Depth, float Stencil) = 0;
			virtual void SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology) = 0;

			virtual void UpdateConstantBuffer(RenderingBackend::ConstantBuffer ConstantBuffer, RenderingBackend::ConstantBufferData Data) = 0;
			virtual void BindConstantBuffer(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::ConstantBuffer const & Buffers, u32 StartIndex) = 0;

			virtual void BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind) = 0;
			virtual void BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex = 0u) = 0;
			virtual void BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex = 0u) = 0;
			virtual void DrawMesh(RenderingBackend::MeshProxy Mesh) = 0;

			virtual void Present() = 0;


		protected:
			darray<Vertex> const & GetVertices(Funky::RawMesh const * Mesh);
			darray<unsigned short> const & GetIndices(Funky::RawMesh const * Mesh);
		

		};
	}
}