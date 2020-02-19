#pragma once

#include "Core/Memory/UniquePtr.h"

#include "Math/Vector3.h"
#include "Math/Vector2.h"

#include "BasicTypes.h"

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
		class RenderingBackend
		{
		public:
			enum class API
			{
				UNKNOWN,
				DX11,
				OGL
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

			static constexpr u64 INVALID_INDEX = (u64)-1;
			
			FK_DEFINE_RENDERINGBACKEND_TYPE(RenderTarget, u64);
			FK_DEFINE_RENDERINGBACKEND_TYPE(PixelShader, u64);
			FK_DEFINE_RENDERINGBACKEND_TYPE(VertexShader, u64);

			FK_DEFINE_RENDERINGBACKEND_TYPE(Texture, u64);
			FK_DEFINE_RENDERINGBACKEND_TYPE(StaticMesh, u64);

			RenderingBackend(API Api);
			~RenderingBackend();

			bool Init(HWND hwnd);
			void OnViewportResized(Math::Vec2u const & NewSize);

			RenderingBackend::API GetBackendAPI() const;

			RenderTarget CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/);

			VertexShader CreateVertexShader(byte* VertexShaderData, u64 DataSize);
			PixelShader CreatePixelShader(byte* PixelShaderData, u64 DataSize);

			RBuffer* CreateBuffer(size SizeOfBuffer, RBuffer::Type BufferType, RBuffer::UsageType Usage, RBuffer::Data_t Data = nullptr);


			Texture CreateTexture2D(byte const * const Data, Math::Vec2u const & Size);
			Texture CreateCubemap(byte const * const Data, Math::Vec2u const & Size);

			void BindRenderTarget(RenderTarget RenderTargetToBind);

			void BindDefaultRenderTarget();
			void ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget const & RenderTargetToClear = RenderingBackend::INVALID_INDEX);
			void ClearDepthStencil(float Depth, float Stencil);
			void SetPrimitiveTopology(PrimitiveTopology NewTopology);

			void UpdateConstantBuffer(RBuffer* const Buffer, RBuffer::Data_t Data);
			void BindConstantBuffer(ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex = 0u);

			void BindVertexShader(VertexShader VertexShaderToBind);
			void BindPixelShader(PixelShader PixelShaderToBind);
			void BindTexture(ShaderResourceStage Stage, Texture const & Texture, u32 StartIndex = 0u);
			void BindTexture(ShaderResourceStage Stage, RenderTarget const & Texture, u32 StartIndex = 0u);

			void Present();


		private:
			class RenderingBackendImpl* Impl = nullptr;

		};
	}
}

#define DEFINE_CONSTANT_BUFFER_BEGIN(BufferName) \
		struct BufferName {

//https://docs.microsoft.com/pl-pl/windows/desktop/direct3dhlsl/dx-graphics-hlsl-packing-rules			
#define DEFINE_CONSTANT_BUFFER_END(BufferName)	\
		};										\
		static_assert((sizeof(BufferName) % 16) == 0, "Constant Buffer size must be 16-byte aligned");	