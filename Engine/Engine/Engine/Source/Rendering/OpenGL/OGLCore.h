#pragma once
#include "Rendering/RenderingBackend.h"
#include "Rendering/RenderingBackendImpl.h"

#include "3rd/gl3w/include/GL/gl3w.h"
#include "Core/Containers.h"

#include "Math/Vector2.h"


/*
 *		It's not fully implemented
 *		And it may crash, actually it will crash for sure.
 *		If you can read this, that means you have to use DX11 implementation :C
 */

namespace Funky
{
	namespace Rendering
	{
		class RawMesh;

		struct OGL final : public RenderingBackendImpl
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

			struct OGLGPUMesh : GPUMesh
			{
				GLuint VAO;
				GLuint VBOs[4];

				u64 IndicesCount = 0;

				operator GLuint() const
				{
					return VAO;
				}
			};

			struct OGLGPUTexture : GPUTexture
			{
				GLuint Texture;

				operator GLuint() const
				{
					return Texture;
				}
			};

			struct OGLGPUVertexShader : GPUVertexShader
			{
				GLuint Program;

				operator GLuint() const
				{
					return Program;
				}

			};

			struct OGLGPUPixelShader : GPUPixelShader
			{
				GLuint Shader;

				operator GLuint() const
				{
					return Shader;
				}
			};

			struct OGLGPUConstantBuffer : GPUConstantBuffer
			{
				GLuint Shader;

				operator GLuint() const
				{
					return Shader;
				}
			};

			struct OGLGPURenderTarget : GPURenderTarget
			{
				GLuint Texture;

				operator GLuint() const
				{
					return Texture;
				}
			};

			// pBackBuffer = nullptr;
			// pDepthStencilBuffer = nullptr;

			OGLGPURenderTarget pRenderTarget;
			OGLGPURenderTarget pDepthStencilView;

			darray<OGLGPUConstantBuffer>	ConstantBuffers;
			darray<OGLGPUVertexShader>		VertexShaders;
			darray<OGLGPUPixelShader>		PixelShaders;

			darray<OGLGPUTexture> Textures;
			darray<OGLGPUMesh> Meshes;
			darray<OGLGPURenderTarget> RenderTargets;
		};
	}
}