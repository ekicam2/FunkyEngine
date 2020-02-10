#include "RenderingBackend/RenderingBackend.h"
#include "DX11/DX11Core.h"

namespace Funky
{
	namespace Rendering
	{
		RenderingBackend::RenderingBackend(API Api)
		{
			switch (Api)
			{
			case API::DX11:
				Impl = new Funky::Rendering::DX11();
				break;
			}
		}

		RenderingBackend::API RenderingBackend::GetBackendAPI() const { return Impl->GetBackendAPI(); }

		RenderingBackend::RenderTarget RenderingBackend::CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			return Impl->CreateRenderTarget(Size);
		}

		RenderingBackend::VertexShader RenderingBackend::CreateVertexShader(byte * VertexShaderData, u64 DataSize)
		{
			return Impl->CreateVertexShader(VertexShaderData, DataSize);
		}

		RenderingBackend::PixelShader RenderingBackend::CreatePixelShader(byte * PixelShaderData, u64 DataSize)
		{
			return Impl->CreatePixelShader(PixelShaderData, DataSize);
		}

		bool RenderingBackend::Init(HWND hwnd) { return Impl->Init(hwnd); }

		void RenderingBackend::OnViewportResized(Math::Vec2u const & NewSize)
		{
			Impl->OnViewportResized(NewSize);
		}

		void RenderingBackend::BindDefaultRenderTarget()
		{
			Impl->BindDefaultRenderTarget();
		}

		RenderingBackend::ConstantBuffer RenderingBackend::CreateConstantBuffer(size SizeOfConstantBuffer, [[maybe_unused]]ConstantBufferData InitData)
		{
			return Impl->CreateConstantBuffer(SizeOfConstantBuffer);
		}

		RenderingBackend::Texture RenderingBackend::CreateTexture2D(byte const * const Data, Math::Vec2u const & Size)
		{
			return Impl->CreateTexture2D(Data, Size);
		}

		RenderingBackend::Texture RenderingBackend::CreateCubemap(byte const * const Data, Math::Vec2u const & Size)
		{
			return Impl->CreateCubemap(Data, Size);
		}

		void RenderingBackend::BindRenderTarget(RenderTarget RenderTargetToBind)
		{
			Impl->BindRenderTarget(RenderTargetToBind);
		}

		void RenderingBackend::ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget const & RenderTargetToClear /*= RenderingBackend::INVALID_INDEX*/)
		{
			Impl->ClearRenderTargetWithColor(Color, RenderTargetToClear);
		}

		void RenderingBackend::ClearDepthStencil(float Depth, float Stencil)
		{
			Impl->ClearDepthStencil(Depth, Stencil);
		}

		void RenderingBackend::SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology)
		{
			ASSERT(NewTopology != RenderingBackend::PrimitiveTopology::UNKNOWN, TEXT("Using UNKNOWN primitive topology is forbidden!"));
			Impl->SetPrimitiveTopology(NewTopology);
		}

		void RenderingBackend::UpdateConstantBuffer(RenderingBackend::ConstantBuffer ConstantBuffer, RenderingBackend::ConstantBufferData Data)
		{
			Impl->UpdateConstantBuffer(ConstantBuffer, Data);
		}

		void RenderingBackend::BindConstantBuffer(ShaderResourceStage Stage, RenderingBackend::ConstantBuffer const & Buffers, u32 StartIndex)
		{
			Impl->BindConstantBuffer(Stage, Buffers, StartIndex);
		}

		void RenderingBackend::BindVertexShader(VertexShader VertexShaderToBind)
		{
			Impl->BindVertexShader(VertexShaderToBind);
		}

		void RenderingBackend::BindPixelShader(PixelShader PixelShaderToBind)
		{
			Impl->BindPixelShader(PixelShaderToBind);
		}

		void RenderingBackend::BindTexture(ShaderResourceStage Stage, Texture const & Texture, u32 StartIndex /*= 0u*/)
		{
			Impl->BindTexture(Stage, Texture, StartIndex);
		}

		void RenderingBackend::BindTexture(ShaderResourceStage Stage, RenderTarget const & Texture, u32 StartIndex /*= 0u*/)
		{
			Impl->BindTexture(Stage, Texture, StartIndex);
		}

		void RenderingBackend::Present()
		{
			Impl->Present();
		}
	}
}