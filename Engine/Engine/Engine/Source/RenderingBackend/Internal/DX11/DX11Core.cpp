#include "RenderingBackend/Internal/DX11/DX11Core.h"

#include "LogMacros.h"

#include <d3d11_1.h>
#include "atlbase.h"
#include "Templates.h"

#include "RenderingBackend/Internal/DX11/DirectUtils.h"
#include "RenderingBackend/Internal/DX11/DX11RenderingResources.h"

#include "DX11Marker.h"


namespace Funky
{
	namespace Rendering
	{
		bool DX11::Init(RenderingBackend::RenderingBackendInitDesc* InInitDesc)
		{
			CHECK(InInitDesc->Api == RenderingBackend::EAPI::DX11);

			DX11RenderingInitDesc* InitDesc = static_cast<DX11RenderingInitDesc*>(InInitDesc);

			ResourceManager.Reset(new RenderingResourcesManager());

			if (CreateDeviceAndSwapchain(InitDesc->hWnd))
			{
				return InitSwapchain();
			}

			return false;
		}

		RenderingBackend::EAPI DX11::GetBackendAPI() const
		{
			return RenderingBackend::EAPI::DX11;
		}

		void DX11::OnViewportResized(Math::Vec2u const & NewSize)
		{
			
			// ID3D11RenderTargetView* RTs[] = { nullptr };
			// pDeviceContext->OMSetRenderTargets(0, RTs, nullptr);

			ResourceManager->ReleaseSwapchain();

			pSwapChain->ResizeBuffers(0, NewSize.X, NewSize.Y, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

			DX11RenderTarget* RT = new DX11RenderTarget();
			RT->Size = NewSize;

			D3D11_TEXTURE2D_DESC RenderTargetTextureDesc;
			ZeroMemory(&RenderTargetTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			RenderTargetTextureDesc.Width = NewSize.X;
			RenderTargetTextureDesc.Height = NewSize.Y;
			RenderTargetTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
			RenderTargetTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
			RenderTargetTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			RenderTargetTextureDesc.MipLevels = RenderTargetTextureDesc.ArraySize = 1;
			RenderTargetTextureDesc.SampleDesc.Count = 1;

			D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
			ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			RenderTargetViewDesc.Format = RenderTargetTextureDesc.Format;
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetViewDesc.Texture2D.MipSlice = 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC RenderTargetTextureShaderViewDesc;
			ZeroMemory(&RenderTargetTextureShaderViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			RenderTargetTextureShaderViewDesc.Format = RenderTargetTextureDesc.Format;
			RenderTargetTextureShaderViewDesc.ViewDimension = D3D_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			RenderTargetTextureShaderViewDesc.Texture2D.MostDetailedMip = 0;
			RenderTargetTextureShaderViewDesc.Texture2D.MipLevels = 1;


			//HRESULT hr = pDevice->CreateTexture2D(&RenderTargetTextureDesc, nullptr, RT->pTexture.GetAddressOf());
			HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&RT->pTexture);

			ASSERT(SUCCEEDED(hr), TEXT("couldn't create render target"));

			hr = pDevice->CreateRenderTargetView(RT->pTexture.Get(), &RenderTargetViewDesc, RT->pRenderTargetView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create render target view");

			D3D11_VIEWPORT Viewport;
			ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
			Viewport.Width = static_cast<float>(NewSize.X);
			Viewport.Height = static_cast<float>(NewSize.Y);
			Viewport.MinDepth = 0.0f;
			Viewport.MaxDepth = 1.0f;

			pDeviceContext->RSSetViewports(1, &Viewport);

			// hr = pDevice->CreateShaderResourceView(RT->pTexture.Get(), &RenderTargetTextureShaderViewDesc, RT->pTextureView.GetAddressOf());
			// ASSERT(SUCCEEDED(hr), L"couldn't create render target texture shader resource view");


			DX11DepthStencil* DS = new DX11DepthStencil();

			//D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
			//ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			//DepthStencilDesc.DepthEnable = TRUE;
			//DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

			D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
			ZeroMemory(&DepthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilTextureDesc.Width = NewSize.X;
			DepthStencilTextureDesc.Height = NewSize.Y;
			DepthStencilTextureDesc.ArraySize = 1;
			DepthStencilTextureDesc.MipLevels = 1;
			DepthStencilTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
			DepthStencilTextureDesc.SampleDesc.Count = 1;

			hr = pDevice->CreateTexture2D(&DepthStencilTextureDesc, nullptr, DS->pTexture.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil buffer"));
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
			ZeroMemory(&DepthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			DepthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;

			pDevice->CreateDepthStencilView(DS->pTexture.Get(), &DepthViewDesc, DS->pDepthStencilView.GetAddressOf());

			ResourceManager->AssociateSwapchain(RT, DS);
			
		}

		RRenderTarget* DX11::CreateRenderTarget(Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			DX11RenderTarget* RT = ResourceManager->RegisterResource<DX11RenderTarget>();
			RT->Size = Size;

			D3D11_TEXTURE2D_DESC RenderTargetTextureDesc;
			ZeroMemory(&RenderTargetTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			RenderTargetTextureDesc.Width = Size.X;
			RenderTargetTextureDesc.Height = Size.Y;
			RenderTargetTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
			RenderTargetTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
			RenderTargetTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			RenderTargetTextureDesc.MipLevels = RenderTargetTextureDesc.ArraySize = 1;
			RenderTargetTextureDesc.SampleDesc.Count = 1;

			D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
			ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			RenderTargetViewDesc.Format = RenderTargetTextureDesc.Format;
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetViewDesc.Texture2D.MipSlice = 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC RenderTargetTextureShaderViewDesc;
			ZeroMemory(&RenderTargetTextureShaderViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			RenderTargetTextureShaderViewDesc.Format = RenderTargetTextureDesc.Format;
			RenderTargetTextureShaderViewDesc.ViewDimension = D3D_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			RenderTargetTextureShaderViewDesc.Texture2D.MostDetailedMip = 0;
			RenderTargetTextureShaderViewDesc.Texture2D.MipLevels = 1;


			HRESULT hr = pDevice->CreateTexture2D(&RenderTargetTextureDesc, nullptr, RT->pTexture.GetAddressOf());
			ASSERT(SUCCEEDED(hr), TEXT("couldn't create render target"));

			hr = pDevice->CreateRenderTargetView(RT->pTexture.Get(), &RenderTargetViewDesc, RT->pRenderTargetView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create render target view");

			hr = pDevice->CreateShaderResourceView(RT->pTexture.Get(), &RenderTargetTextureShaderViewDesc, RT->pTextureView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create render target texture shader resource view");

			return RT;
		}

		RDepthStencil* DX11::CreateDepthStencil(Math::Vec2u const& Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			DX11DepthStencil* DS = ResourceManager->RegisterResource<DX11DepthStencil>();

			 //D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
			 //ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			 //DepthStencilDesc.DepthEnable = TRUE;
			 //DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

			D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
			ZeroMemory(&DepthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilTextureDesc.Width = Size.X;
			DepthStencilTextureDesc.Height = Size.Y;
			DepthStencilTextureDesc.ArraySize = 1;
			DepthStencilTextureDesc.MipLevels = 1;
			DepthStencilTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
			DepthStencilTextureDesc.SampleDesc.Count = 1;
			
			HRESULT hr = pDevice->CreateTexture2D(&DepthStencilTextureDesc, nullptr, DS->pTexture.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil buffer"));
				return nullptr;
			}
			
			D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
			ZeroMemory(&DepthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			DepthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
			
			pDevice->CreateDepthStencilView(DS->pTexture.Get(), &DepthViewDesc, DS->pDepthStencilView.GetAddressOf());

			return DS;
		}

		RShader* DX11::CreateVertexShader(RenderingBackend::ShaderInputDesc* ShaderDesc)
		{
			DX11VertexShader* Shader = ResourceManager->RegisterResource<DX11VertexShader>();

			HRESULT hr = pDevice->CreateVertexShader(ShaderDesc->ShaderData, ShaderDesc->DataSize, nullptr, Shader->pVs.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create simple vertex shader");

			darray<D3D11_INPUT_ELEMENT_DESC> LayoutDesc =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			hr = pDevice->CreateInputLayout(LayoutDesc.data(), (u32)LayoutDesc.size(), ShaderDesc->ShaderData, ShaderDesc->DataSize, Shader->pInputLayout.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create input layout");

			return Shader;
		}

		RShader* DX11::CreatePixelShader(RenderingBackend::ShaderInputDesc* ShaderDesc)
		{
			DX11PixelShader* Shader = ResourceManager->RegisterResource<DX11PixelShader>();

			HRESULT hr = pDevice->CreatePixelShader(ShaderDesc->ShaderData, ShaderDesc->DataSize, nullptr, Shader->pPs.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create simple pixel shader");

			return Shader;
		}

		RBuffer* DX11::CreateBuffer(size SizeOfBuffer, RBuffer::EType BufferType, RBuffer::EUsageType Usage, RBuffer::Data_t Data /*= nullptr*/, size Stride /*= 0u*/)
		{
			if(Usage == RBuffer::EUsageType::Static)
				CHECK(Data != nullptr);

			D3D11_BUFFER_DESC ConstantBufferDesc;
			ZeroMemory(&ConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));
			ConstantBufferDesc.ByteWidth = (u32)SizeOfBuffer;
			ConstantBufferDesc.BindFlags = DirectUtils::BufferToDXType(BufferType);
			ConstantBufferDesc.Usage = DirectUtils::BufferToDXUsage(Usage);
			ConstantBufferDesc.CPUAccessFlags = Usage == RBuffer::EUsageType::Static ? 0 : D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
			
			D3D11_SUBRESOURCE_DATA BufferInitData;
			ZeroMemory(&BufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));
			BufferInitData.pSysMem = Data;

			DX11Buffer* Ret = ResourceManager->RegisterResource<DX11Buffer>(BufferType, Usage);
			Ret->SizeInBytes = SizeOfBuffer;
			Ret->Stride = [&BufferType, &Data, &Stride]() -> u32 
			{
				// if it seems ok it is ok
				if (Data != nullptr && Stride != 0u) return (u32)Stride;
				// if Vertex and has no Stride apply default
				else if (BufferType == RBuffer::EType::Vertex && Stride == 0u) return sizeof(Vertex);
				// if Index it HAS to be u16
				else if (BufferType == RBuffer::EType::Index) return sizeof(u16);
					
				// if Uniform don't realy care for the stride however return 1
				// to prevent division by 0 when calculating @ElementCount
				return 1u;
			}(); 
			Ret->ElementCount = (u32)SizeOfBuffer / Ret->Stride;
			
			HRESULT hr = pDevice->CreateBuffer(&ConstantBufferDesc, Data ? &BufferInitData : nullptr, Ret->pBuffer.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create buffer");

			return Ret;
		}

		RTexture* DX11::CreateTexture2D([[maybe_unused]]byte const * const Data, [[maybe_unused]] Math::Vec2u const & Size)
		{
		/*	
			Data; Size;
			Textures.push_back(Move(
				[Size]()
				{
					DX11GPUTexture Returner;
					ZeroMemory(&Returner.TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
					Returner.TextureDesc.Width = Size.X;
					Returner.TextureDesc.Height = Size.Y;
					Returner.TextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
					Returner.TextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
					Returner.TextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
					Returner.TextureDesc.MipLevels = Returner.TextureDesc.ArraySize = 1;
					Returner.TextureDesc.SampleDesc.Count = 1;
					return Returner;
				}()
			));

			const u64 Index = Textures.size() - 1;

			D3D11_SUBRESOURCE_DATA TextureResourceData;
			ZeroMemory(&TextureResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
			TextureResourceData.pSysMem = Data;
			TextureResourceData.SysMemPitch = sizeof(char) * 4 * Size.X;

			HRESULT hr = pDevice->CreateTexture2D(&Textures[Index].TextureDesc, &TextureResourceData, Textures[Index].pTexture.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create texture");

			D3D11_SHADER_RESOURCE_VIEW_DESC TextureView;
			TextureView.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			TextureView.ViewDimension = D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D;
			TextureView.Texture2D.MipLevels = 1;
			TextureView.Texture2D.MostDetailedMip = 0;

			hr = pDevice->CreateShaderResourceView(Textures[Index].pTexture.Get(), &TextureView, Textures[Index].pTextureView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create texture view");

			return Index;*/

			return nullptr;
		}

		RTexture* DX11::CreateCubemap([[maybe_unused]] byte const * const Data, [[maybe_unused]] Math::Vec2u const & Size)
		{
			/*
			Data, Size;
			constexpr unsigned TexCount = 6u;

			Textures.push_back(Move(
				[Size, TexCount]()
				{
					DX11GPUTexture Returner;
					ZeroMemory(&Returner.TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
					Returner.TextureDesc.Width = Size.X;
					Returner.TextureDesc.Height = Size.Y;
					Returner.TextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
					Returner.TextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
					Returner.TextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
					Returner.TextureDesc.MipLevels = 1;
					Returner.TextureDesc.ArraySize = TexCount;
					Returner.TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
					Returner.TextureDesc.SampleDesc.Count = 1;
					return Returner;
				}()
			));

			const u64 Index = Textures.size() - 1;

			D3D11_SUBRESOURCE_DATA TextureResourceData[TexCount];
			for (int i = 0; i < TexCount; ++i)
			{
				ZeroMemory(&TextureResourceData[i], sizeof(D3D11_SUBRESOURCE_DATA));
				TextureResourceData[i].pSysMem = Data + (i * sizeof(byte) * 4 * Size.X * Size.Y);
				TextureResourceData[i].SysMemPitch = sizeof(byte) * 4 * Size.X;
			}

			HRESULT hr = pDevice->CreateTexture2D(&Textures[Index].TextureDesc, TextureResourceData, Textures[Index].pTexture.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create texture");

			D3D11_SHADER_RESOURCE_VIEW_DESC TextureView;
			TextureView.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			TextureView.ViewDimension = D3D_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
			TextureView.TextureCube.MipLevels = 1;
			TextureView.TextureCube.MostDetailedMip = 0;

			hr = pDevice->CreateShaderResourceView(Textures[Index].pTexture.Get(), &TextureView, Textures[Index].pTextureView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create texture view");

			return Index;*/

			return nullptr;
		}

		void DX11::BindRenderTarget(RRenderTarget* RenderTargetToBind, RDepthStencil* DepthStencilToBind)
		{
			//TODO(ekicam2): it's hacky to unbind it all the time, render target should cache is somehow
			ID3D11ShaderResourceView* rt1[] = { nullptr, nullptr, nullptr };
			pDeviceContext->PSSetShaderResources(0, 3u, rt1);
			pDeviceContext->VSSetShaderResources(0, 3u, rt1);

			DX11RenderTarget* RT = reinterpret_cast<DX11RenderTarget*>(RenderTargetToBind);
			DX11DepthStencil* DS = reinterpret_cast<DX11DepthStencil*>(DepthStencilToBind);

			ID3D11RenderTargetView* rt[] = { RT ? RT->pRenderTargetView.Get() : nullptr };
			pDeviceContext->OMSetRenderTargets(1, rt, DS ? DS->pDepthStencilView.Get() : nullptr);

			D3D11_VIEWPORT Viewport;
			ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
			Viewport.Width = static_cast<float>(RT->Size.X);
			Viewport.Height = static_cast<float>(RT->Size.Y);
			Viewport.MinDepth = 0.0f;
			Viewport.MaxDepth = 1.0f;

			pDeviceContext->RSSetViewports(1, &Viewport);
		}

		void DX11::ClearRenderTarget(RRenderTarget* RenderTargetToClear, Math::Vec3f const& Color)
		{
			float color[4] = { Color.X, Color.Y, Color.Z, 1.0f };

			DX11RenderTarget* RT = static_cast<DX11RenderTarget*>(RenderTargetToClear);
			pDeviceContext->ClearRenderTargetView(RT->pRenderTargetView.Get(), color);
		}

		void DX11::ClearDepthStencil(RDepthStencil* DepthStencilToClear, float Depth, float Stencil, bool bClearDepth, bool bClearStencil)
		{
			CHECK(bClearDepth || bClearStencil);
			
			UINT ClearFlags = 0x0L;

			if (bClearDepth)
				ClearFlags |= D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH;
			if (bClearStencil)
				ClearFlags |= D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL;

			DX11DepthStencil* DepthStencil = static_cast<DX11DepthStencil*>(DepthStencilToClear);
			pDeviceContext->ClearDepthStencilView(DepthStencil->pDepthStencilView.Get(), ClearFlags, Depth, (u8)Stencil);
		}

		void DX11::SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology)
		{
			pDeviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)NewTopology);
		}

		void DX11::UpdateBuffer(RBuffer* ConstantBuffer, RBuffer::Data_t Data)
		{
			D3D11_MAPPED_SUBRESOURCE MappedData;
			ZeroMemory(&MappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			pDeviceContext->Map(static_cast<DX11Buffer*>(ConstantBuffer)->pBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedData);
			memcpy(MappedData.pData, Data, static_cast<DX11Buffer*>(ConstantBuffer)->SizeInBytes);
			pDeviceContext->Unmap(static_cast<DX11Buffer*>(ConstantBuffer)->pBuffer.Get(), 0);
		}

		void DX11::BindBuffer(RenderingBackend::ShaderResourceStage Stage, RBuffer* const Buffer, u32 StartIndex)
		{
			DX11Buffer* DXBuffer = static_cast<DX11Buffer*>(Buffer);

			u32 s = 0, g = 0;

			if (Buffer->BufferType == RBuffer::EType::Vertex)
			{
				pDeviceContext->IASetVertexBuffers(0u, 1u, DXBuffer->pBuffer.GetAddressOf(), &s, &g);
				pDeviceContext->IASetVertexBuffers(0u, 1u, DXBuffer->pBuffer.GetAddressOf(), &s, &g);
			}
			if (Buffer->BufferType == RBuffer::EType::Index)
			{
				pDeviceContext->IASetIndexBuffer(DXBuffer->pBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
			}
			else
			{
				switch (Stage)
				{
				case RenderingBackend::ShaderResourceStage::Fragment:
					pDeviceContext->PSSetConstantBuffers(StartIndex, 1u, (ID3D11Buffer * const *)static_cast<DX11Buffer*>(Buffer)->pBuffer.GetAddressOf());
					break;
				case RenderingBackend::ShaderResourceStage::Vertex:
					pDeviceContext->VSSetConstantBuffers(StartIndex, 1u, (ID3D11Buffer * const *)static_cast<DX11Buffer*>(Buffer)->pBuffer.GetAddressOf());
					break;
				}
			}
		}

		void DX11::BindVertexShader(RShader* VertexShaderToBind)
		{
			DX11VertexShader* DShader = static_cast<DX11VertexShader*>(VertexShaderToBind);

			pDeviceContext->IASetInputLayout(DShader->pInputLayout.Get());
			pDeviceContext->VSSetShader(DShader->pVs.Get(), nullptr, 0);
		}

		void DX11::BindPixelShader(RShader* PixelShaderToBind)
		{
			DX11PixelShader* DShader = static_cast<DX11PixelShader*>(PixelShaderToBind);
			pDeviceContext->PSSetShader(DShader->pPs.Get(), nullptr, 0);
		}

		void DX11::Draw(RBuffer* InVertexBuffer)
		{
			DX11Buffer* VertexBuffer = reinterpret_cast<DX11Buffer*>(InVertexBuffer);

			pDeviceContext->IASetVertexBuffers(0u, 1u, VertexBuffer->pBuffer.GetAddressOf(), &(VertexBuffer->Stride), &(VertexBuffer->Offset));
			pDeviceContext->Draw(VertexBuffer->ElementCount, 0);

			//pDeviceContext->IASetIndexBuffer(Meshes[Mesh].pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
			//pDeviceContext->DrawIndexed((u32)Meshes[Mesh].IndicesCount, 0, 0);
		}


		void DX11::DrawIndexed(RBuffer* InVertexBuffer, RBuffer* InIndexBuffer)
		{
			DX11Buffer* VertexBuffer = reinterpret_cast<DX11Buffer*>(InVertexBuffer);
			DX11Buffer* IndexBuffer = reinterpret_cast<DX11Buffer*>(InIndexBuffer);

			pDeviceContext->IASetVertexBuffers(0u, 1u, VertexBuffer->pBuffer.GetAddressOf(), &(VertexBuffer->Stride), &(VertexBuffer->Offset));

			pDeviceContext->IASetIndexBuffer(IndexBuffer->pBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
			pDeviceContext->DrawIndexed(IndexBuffer->ElementCount, 0u, 0u);
		}


		void DX11::Present()
		{
			pSwapChain->Present(0, 0);
		}


		bool DX11::CreateDeviceAndSwapchain(HWND hwnd)
		{
			D3D_FEATURE_LEVEL pFeatureLevels[] = {
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_11_1
			};

			WindowHandle = hwnd;

			UINT iDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT
				| D3D11_CREATE_DEVICE_SINGLETHREADED //atm we are only on one thread
#ifdef _DEBUG
				| D3D11_CREATE_DEVICE_DEBUG
#endif
				;

			D3D_FEATURE_LEVEL ActuallySupportedLevel;

			HRESULT hr = D3D11CreateDevice(
				nullptr,                    // Specify nullptr to use the default adapter.
				D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
				0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
				iDeviceFlags,                // Set debug and Direct2D compatibility flags.
				pFeatureLevels,                     // List of feature levels this app can support.
				ARRAYSIZE(pFeatureLevels),          // Size of the list above.
				D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
				&pDevice,                    // Returns the Direct3D device created.
				&ActuallySupportedLevel,            // Returns feature level of device created.
				&pDeviceContext                    // Returns the device immediate context.
			);

			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("couldnt create device"));
				return false;
			}

			hr = pDeviceContext->QueryInterface(__uuidof(pDebugMarkersStack), reinterpret_cast<void**>(pDebugMarkersStack.GetAddressOf()));
			if (FAILED(hr))
			{
				LOG_ERROR(TEXT("Couldn't query user defined annotations interface"));
				return false;
			}

			LOG(TEXT("actually supported level is: "), DirectUtils::FeatureNameToString(ActuallySupportedLevel));

			return true;
		}

		bool DX11::InitSwapchain()
		{
			Microsoft::WRL::ComPtr<IDXGIDevice> Device;
			pDevice.As(&Device);

			Microsoft::WRL::ComPtr<IDXGIAdapter> Adapter;
			Device->GetAdapter(Adapter.GetAddressOf());

			Microsoft::WRL::ComPtr<IDXGIFactory> Factory;
			HRESULT hr = Adapter->GetParent(IID_PPV_ARGS(&Factory));
			ASSERT(SUCCEEDED(hr), TEXT("couldn't get adapter factory"));

			RECT ClientSize;
			GetClientRect(WindowHandle, &ClientSize);

			WindowSize.X = ClientSize.right - ClientSize.left;
			WindowSize.Y = ClientSize.bottom - ClientSize.top;

			DXGI_SWAP_CHAIN_DESC SwapChainDesc;
			ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

			SwapChainDesc.Windowed = TRUE;
			SwapChainDesc.SampleDesc.Count = 1u;
			SwapChainDesc.SampleDesc.Quality = 0u;
			SwapChainDesc.BufferCount = 3u;
			SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDesc.BufferDesc.Width = WindowSize.X;
			SwapChainDesc.BufferDesc.Height = WindowSize.Y;
			SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1u;
			SwapChainDesc.BufferDesc.RefreshRate.Denominator = 75u;
			SwapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
			SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE; // dk
			SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_CENTERED;
			SwapChainDesc.OutputWindow = WindowHandle;
			SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			if (SUCCEEDED(hr))
			{
				hr = Factory->CreateSwapChain(pDevice.Get(), &SwapChainDesc, pSwapChain.GetAddressOf());
				ASSERT(SUCCEEDED(hr), TEXT("couldn't create swapchain"));
			}

			// FU Microsoft I'll do it myself
			if (SUCCEEDED(hr))
			{
				hr = Factory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
				ASSERT(SUCCEEDED(hr), TEXT("Couldn't associate swapchain with window."));
			}

			DX11RenderTarget* RT = new DX11RenderTarget();
			hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&RT->pTexture);
			RT->Size.X = WindowSize.X;
			RT->Size.Y = WindowSize.Y;

			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("couldn't get back buffer"));
				return false;
			}

			D3D11_TEXTURE2D_DESC BackBufferDesc;
			RT->pTexture->GetDesc(&BackBufferDesc);
			
			hr = pDevice->CreateRenderTargetView(RT->pTexture.Get(), nullptr, RT->pRenderTargetView.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("couldn't create render target"));
				return false;
			}

			DX11DepthStencil* DS = new DX11DepthStencil();

			//D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
			//ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			//DepthStencilDesc.DepthEnable = TRUE;
			//DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

			D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
			ZeroMemory(&DepthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
			DepthStencilTextureDesc.Width = WindowSize.X;
			DepthStencilTextureDesc.Height = WindowSize.Y;
			DepthStencilTextureDesc.ArraySize = 1;
			DepthStencilTextureDesc.MipLevels = 1;
			DepthStencilTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			DepthStencilTextureDesc.SampleDesc.Count = 1;

			hr = pDevice->CreateTexture2D(&DepthStencilTextureDesc, nullptr, DS->pTexture.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil buffer"));
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
			ZeroMemory(&DepthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			DepthViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;

			hr = pDevice->CreateDepthStencilView(DS->pTexture.Get(), &DepthViewDesc, DS->pDepthStencilView.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil texture view"));
				return false;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC DepthTextureViewDEsc;
			DepthTextureViewDEsc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			DepthTextureViewDEsc.ViewDimension = D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D;
			DepthTextureViewDEsc.Texture2D.MipLevels = 1;
			DepthTextureViewDEsc.Texture2D.MostDetailedMip = 0;

			hr = pDevice->CreateShaderResourceView(DS->pTexture.Get(), &DepthTextureViewDEsc, DS->pTextureView.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil view"));
				return false;
			}

			ResourceManager->AssociateSwapchain(RT, DS);

			D3D11_VIEWPORT Viewport;
			ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
			Viewport.Width = static_cast<float>(WindowSize.X);
			Viewport.Height = static_cast<float>(WindowSize.Y);
			Viewport.MinDepth = 0.0f;
			Viewport.MaxDepth = 1.0f;

			pDeviceContext->RSSetViewports(1, &Viewport);

			return true;
		}

		void DX11::FreeSwapchain()
		{
			// BindRenderTarget(nullptr);
			ResourceManager->FreeAll();
		}

		void DX11::BindTexture(RenderingBackend::ShaderResourceStage Stage, RTexture* InTexture, u32 StartIndex /*= 0u*/)
		{
			DX11Texture* Texture = static_cast<DX11Texture*>(InTexture);

			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			}
		}

		void DX11::BindTexture(RenderingBackend::ShaderResourceStage Stage, RDepthStencil* InTexture, u32 StartIndex /*= 0u*/)
		{
			DX11DepthStencil* Texture = static_cast<DX11DepthStencil*>(InTexture);

			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			}
		}

		void DX11::BindTexture(RenderingBackend::ShaderResourceStage Stage, RRenderTarget* InTexture, u32 StartIndex /*= 0u*/)
		{
			DX11RenderTarget* Texture = static_cast<DX11RenderTarget*>(InTexture);

			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetShaderResources(StartIndex, 1u, Texture->pTextureView.GetAddressOf());
				break;
			}
		}

		Funky::Rendering::IGPUMarker* DX11::MarkScope(Str MarkerName)
		{
			return new Rendering::DX11Marker(pDebugMarkersStack.Get(), MarkerName);
		}

	}
}