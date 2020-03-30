#include "RenderingBackend/RenderingBackend.h"

#include "Core/Events/EngineEvents.h"

#include "DX11/DX11Core.h"

namespace Funky
{
	namespace Rendering
	{
		RenderingBackend::RenderingBackend()
		{
			
		}

		RenderingBackend::~RenderingBackend()
		{
			delete Impl;
		}

		RenderingBackend::API RenderingBackend::GetBackendAPI() const { return Impl->GetBackendAPI(); }

		RRenderTarget* RenderingBackend::CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			return Impl->CreateRenderTarget(Size);
		}

		RDepthStencil* RenderingBackend::CreateDepthStencil(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			return Impl->CreateDepthStencil(Size);
		}

		RShader* RenderingBackend::CreateVertexShader(ShaderInputDesc* ShaderDesc)
		{
			return Impl->CreateVertexShader(ShaderDesc);
		}

		RShader* RenderingBackend::CreatePixelShader(ShaderInputDesc* ShaderDesc)
		{
			return Impl->CreatePixelShader(ShaderDesc);
		}

		RBuffer* RenderingBackend::CreateBuffer(size SizeOfBuffer, RBuffer::EType BufferType, RBuffer::EUsageType Usage, RBuffer::Data_t Data /*= nullptr*/, size Stride /*= 0u*/)
		{
			return Impl->CreateBuffer(SizeOfBuffer, BufferType, Usage, Data, Stride);
		}

		bool RenderingBackend::Init(RenderingBackendInitDesc* InitDesc) 
		{ 
			switch (InitDesc->Api)
			{
			case API::DX11:
				delete Impl;
				Impl = new Funky::Rendering::DX11();
				break;
			}

			Funky::OnViewportResized.RegisterLambda([this](Math::Vec2u NewSize) { 
				OnViewportResized(NewSize);
			});
			
			return Impl->Init(InitDesc); 
		}

		void RenderingBackend::OnViewportResized(Math::Vec2u const & NewSize)
		{
			Impl->OnViewportResized(NewSize);
		}

		RTexture* RenderingBackend::CreateTexture2D(byte const* const Data, Math::Vec2u const& Size)
		{
			return Impl->CreateTexture2D(Data, Size);
		}

		RTexture* RenderingBackend::CreateCubemap(byte const* const Data, Math::Vec2u const& Size)
		{
			return Impl->CreateCubemap(Data, Size);
		}

		void RenderingBackend::BindRenderTarget(RRenderTarget* RenderTargetToBind, RDepthStencil* DepthStencilToBind /*= nullptr*/)
		{
			CHECK(RenderTargetToBind->Size != Math::Vec2u(0, 0));

			Impl->BindRenderTarget(RenderTargetToBind, DepthStencilToBind);
		}

		void RenderingBackend::ClearRenderTarget(RRenderTarget* RenderTargetToClear, Math::Vec3f const& Color)
		{
			Impl->ClearRenderTarget(RenderTargetToClear, Color);
		}

		void RenderingBackend::ClearDepthStencil(RDepthStencil* DepthStencilToClear, float Depth, float Stencil, bool bClearDepth /*= true*/, bool bClearStencil /*= true*/)
		{
			Impl->ClearDepthStencil(DepthStencilToClear, Depth, Stencil, bClearDepth, bClearStencil);
		}

		void RenderingBackend::SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology)
		{
			ASSERT(NewTopology != RenderingBackend::PrimitiveTopology::UNKNOWN, TEXT("Using UNKNOWN primitive topology is forbidden!"));
			Impl->SetPrimitiveTopology(NewTopology);
		}

		void RenderingBackend::UpdateConstantBuffer(RBuffer* const Buffer, RBuffer::Data_t Data)
		{
			Impl->UpdateBuffer(Buffer, Data);
		}


		void RenderingBackend::BindConstantBuffer(ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex /*= 0u*/)
		{
			Impl->BindBuffer(Stage, Buffer, StartIndex);
		}

		void RenderingBackend::BindVertexShader(RShader* VertexShaderToBind)
		{
			CHECK(VertexShaderToBind->ShaderType == RShader::Type::Vertex);

			Impl->BindVertexShader(VertexShaderToBind);
		}

		void RenderingBackend::BindPixelShader(RShader* PixelShaderToBind)
		{
			CHECK(PixelShaderToBind->ShaderType == RShader::Type::Pixel);

			Impl->BindPixelShader(PixelShaderToBind);
		}

		void RenderingBackend::BindTexture(ShaderResourceStage Stage, RTexture* Texture, u32 StartIndex /*= 0u*/)
		{
			Impl->BindTexture(Stage, Texture, StartIndex);
		}

		void RenderingBackend::BindTexture(ShaderResourceStage Stage, RRenderTarget* Texture, u32 StartIndex /*= 0u*/)
		{
			Impl->BindTexture(Stage, Texture, StartIndex);
		}

		void RenderingBackend::Draw(RBuffer* VertexBuffer)
		{
			CHECK(VertexBuffer->BufferType == RBuffer::EType::Vertex);

			Impl->Draw(VertexBuffer);
		}

		void RenderingBackend::DrawIndexed(RBuffer* VertexBuffer, RBuffer* IndexBuffer)
		{
			CHECK(VertexBuffer->BufferType == RBuffer::EType::Vertex);
			CHECK(IndexBuffer->BufferType == RBuffer::EType::Index);

			Impl->DrawIndexed(VertexBuffer, IndexBuffer);
		}

		void RenderingBackend::Present()
		{
			Impl->Present();
		}

		RenderingResourcesManager* RenderingBackend::GetResourceManager()
		{
			return Impl->ResourceManager;
		}

	}
}