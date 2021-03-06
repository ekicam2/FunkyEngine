#pragma once

#include "Core/Memory/UniquePtr.h"

#include "Math/Vector3.h"
#include "Math/Vector2.h"

#include "BasicTypes.h"
#include "Core/String.h"

#include "Internal/RenderingBackendUtils.h"

#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "RenderingFrontend/IRenderingResource.h"

/* 
 *	Constant buffer declaration:
 * --------------------------------
 *
 * DEFINE_CONSTANT_BUFFER_BEGIN(BaseConstantBuffer)
 * BaseConstantBuffer() : LookAt(0.0f, 0.0f, 0.0f) {}
 * DirectX::XMMATRIX MVP;
 * DirectX::XMMATRIX Model;
 * Funky::Math::Vec3f LookAt;
 * DEFINE_CONSTANT_BUFFER_END(BaseConstantBuffer)
 *
 */

namespace Funky
{
	namespace Rendering
	{
		class IGPUMarker;

		class RenderingBackend
		{
		public:
			enum class EAPI
			{
				UNKNOWN,
				DX11,
				//OGL
			};

			enum class ShaderResourceStage
			{
				Vertex,
				Fragment
			};

			enum class PrimitiveTopology
			{
				UNKNOWN = 0,
				Pointlist = 1,
				Linelist = 2,
				Linestrip = 3,
				Trianglelist = 4,
				Trianglestrip = 5,
				LinelistAdj = 10,
				LinestripAdj = 11,
				TrianglelistAdj = 12,
				TrianglestripAdj = 13
			};

			struct RenderingBackendInitDesc
			{
				EAPI Api;
			};

			struct RenderingBackendInitResult
			{
				RRenderTarget* SwapchainRT;
				RDepthStencil* SwapchainDS;
			};

			struct ShaderInputDesc
			{
				byte* ShaderData;
				u64 DataSize;
			};

			RenderingBackend();
			virtual ~RenderingBackend();

			FORCEINLINE bool IsInitialized() const { return Impl != nullptr; }

			bool Init(RenderingBackendInitDesc* InitDesc, RenderingBackendInitResult* Result);
			void OnViewportResized(Math::Vec2u const & NewSize);

			RenderingBackend::EAPI GetBackendAPI() const;

			RRenderTarget* CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/);
			RDepthStencil* CreateDepthStencil(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/);

			RShader* CreateVertexShader(ShaderInputDesc* ShaderDesc);
			RShader* CreatePixelShader(ShaderInputDesc* ShaderDesc);

			RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::EType BufferType, RBuffer::EUsageType Usage, RBuffer::Data_t Data = nullptr, size Stride = 0u);

			RTexture* CreateTexture2D(byte const * const Data, Math::Vec2u const & Size);
			RTexture* CreateCubemap(byte const * const Data, Math::Vec2u const & Size);

			void BindRenderTarget(RRenderTarget* RenderTargetToBind, RDepthStencil* DepthStencilToBind = nullptr);

			void ClearRenderTarget(RRenderTarget* RenderTargetToClear, Math::Vec3f const & Color);
			void ClearDepthStencil(RDepthStencil* DepthStencilToClear, float Depth, float Stencil, bool bClearDepth = true, bool bClearStencil = true);
			void SetPrimitiveTopology(PrimitiveTopology NewTopology);

			void UpdateConstantBuffer(RBuffer* const Buffer, RBuffer::Data_t Data);
			void BindConstantBuffer(ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex = 0u);

			void BindVertexShader(RShader* VertexShaderToBind);
			void BindPixelShader(RShader* PixelShaderToBind);
			void BindTexture(ShaderResourceStage Stage, RTexture* Texture, u32 StartIndex = 0u);
			void BindTexture(ShaderResourceStage Stage, RDepthStencil* Texture, u32 StartIndex = 0u);
			void BindTexture(ShaderResourceStage Stage, RRenderTarget* Texture, u32 StartIndex = 0u);

			void Draw(RBuffer* VertexBuffer);
			void DrawIndexed(RBuffer* VertexBuffer, RBuffer* IndexBuffer);

			void Present();

			Rendering::IGPUMarker* MarkScope(Str MarkerName);

		private:
			class RenderingBackendImpl* Impl = nullptr;

		};
	
		struct DX11RenderingInitDesc : public RenderingBackend::RenderingBackendInitDesc
		{
			HWND hWnd;
		};
	}
}