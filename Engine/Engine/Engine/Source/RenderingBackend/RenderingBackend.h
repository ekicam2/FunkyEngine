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

			enum class EDepthFunc : u8
			{
				Less,
				Greater
			};

			template <bool Enabled, bool ReadOnly, EDepthFunc InDepth>
			struct DepthState
			{
				static constexpr bool bEnabled = Enabled;
				static constexpr bool bReadOnly = ReadOnly;
				static constexpr EDepthFunc DepthFunc = InDepth;


				static DepthState& Get()
				{
					static DepthState Singleton;
					return Singleton;
				}
			};

			struct RenderingBackendInitDesc
			{
				API Api;
			};

			struct ShaderInputDesc
			{
				byte* ShaderData;
				u64 DataSize;
			};

			RenderingBackend();
			virtual ~RenderingBackend();

			bool IsInitialized() const {
				return Impl != nullptr;
			}

			bool Init(RenderingBackendInitDesc* InitDesc);
			void OnViewportResized(Math::Vec2u const & NewSize);

			RenderingBackend::API GetBackendAPI() const;

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
			void BindTexture(ShaderResourceStage Stage, RRenderTarget* Texture, u32 StartIndex = 0u);

			void Draw(RBuffer* VertexBuffer);
			void DrawIndexed(RBuffer* VertexBuffer, RBuffer* IndexBuffer);

			void Present();

			class RenderingResourcesManager* GetResourceManager();


		private:
			class RenderingBackendImpl* Impl = nullptr;

		};
	
		struct DX11RenderingInitDesc : public RenderingBackend::RenderingBackendInitDesc
		{
			HWND hWnd;
		};
	}
}