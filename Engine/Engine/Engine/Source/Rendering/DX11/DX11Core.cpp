#include "Rendering/DX11/DX11Core.h"
#include "Rendering/RawMesh.h"

#include "LogMacros.h"

#include "Rendering/DX11/DX11ImGUIFasade.h"
#include <d3d11_1.h>
#include "atlbase.h"
#include "Templates.h"

namespace Funky
{
	namespace Rendering
	{
		bool DX11::Init(HWND hwnd)
		{
			if (CreateDeviceAndSwapchain(hwnd))
			{
				ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());

				return InitSwapchain();
			}

			return false;
		}

		RenderingBackend::API DX11::GetBackendAPI() const
		{
			return RenderingBackend::API::DX11;
		}

		void DX11::OnViewportResized([[maybe_unused]]Math::Vector2u const & NewSize)
		{
			pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			
			pRenderTarget->Release();
			pDepthStencilView->Release();

			pSwapChain->ResizeBuffers(0, 0, 0,DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);
			
			// init  
			InitSwapchain();
			
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());
		}

		RenderingBackend::RenderTarget DX11::CreateRenderTarget(Math::Vector2u const & Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			 pTexture = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   pRenderTargetView = nullptr;

			RenderTargets.push_back(Move(DX11GPURenderTarget()));

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

			const u64 Index = RenderTargets.size() - 1;

			HRESULT hr = pDevice->CreateTexture2D(&RenderTargetTextureDesc, nullptr, RenderTargets[Index].pTexture.GetAddressOf());
			ASSERT(SUCCEEDED(hr), TEXT("couldn't create render target"));

			hr = pDevice->CreateRenderTargetView(RenderTargets[Index].pTexture.Get(), &RenderTargetViewDesc, RenderTargets[Index].pRenderTargetView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create render target view");

			hr = pDevice->CreateShaderResourceView(RenderTargets[Index].pTexture.Get(), &RenderTargetTextureShaderViewDesc, RenderTargets[Index].pTextureView.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"couldn't create render target texture shader resource view");

			return Index;
		}

		RenderingBackend::VertexShader DX11::CreateVertexShader(byte* VertexShaderData, u64 DataSize)
		{
			DX11::DX11GPUVertexShader Returner;
			//TODO(ekicam2): consume inputlayout passed from outside
			Returner.InputLayoutDesc = DirectUtils::InputLayout::PositionColorNormalUV;

			HRESULT hr = pDevice->CreateVertexShader(VertexShaderData, DataSize, nullptr, Returner.pVertexShader.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create simple vertex shader");

			darray<D3D11_INPUT_ELEMENT_DESC> InputLayoutDesc;
			DirectUtils::GetInputLayoutDesc(Returner.InputLayoutDesc, InputLayoutDesc);

			hr = pDevice->CreateInputLayout(InputLayoutDesc.data(), (u32)InputLayoutDesc.size(), VertexShaderData, DataSize, Returner.pInputLayout.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create input layout");

			if (hr == S_OK)
			{
				VertexShaders.push_back(Move(Returner));
				return VertexShaders.size() - 1;
			}

			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::PixelShader DX11::CreatePixelShader(byte* PixelShaderData, u64 DataSize)
		{
			DX11::DX11GPUPixelShader Returner;

			HRESULT hr = pDevice->CreatePixelShader(PixelShaderData, DataSize, nullptr, Returner.pPixelShader.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create simple pixel shader");

			if (hr == S_OK)
			{
				PixelShaders.push_back(Move(Returner));
				return PixelShaders.size() - 1;
			}

			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::ConstantBuffer DX11::CreateConstantBuffer(size SizeOfConstantBuffer)
		{
			D3D11_BUFFER_DESC ConstantBufferDesc;
			ZeroMemory(&ConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));
			ConstantBufferDesc.ByteWidth = (u32)SizeOfConstantBuffer;
			ConstantBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			ConstantBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

			ConstantBuffers.push_back(
				[SizeOfConstantBuffer]
				{
					DX11GPUConstantBuffer Returner;
					Returner.SizeInBytes = SizeOfConstantBuffer;
					Returner.pConstantBuffer = nullptr;
					return Returner;
				}()
			);
			
			const u64 Index = ConstantBuffers.size() - 1;

			HRESULT hr = pDevice->CreateBuffer(&ConstantBufferDesc, nullptr, ConstantBuffers[Index].pConstantBuffer.GetAddressOf());
			ASSERT(SUCCEEDED(hr), L"Couldn't create constant buffer");

			return Index;
		}

		RenderingBackend::Texture DX11::CreateTexture2D(byte const * const Data, Math::Vector2u const & Size)
		{
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

			return Index;
		}

		RenderingBackend::Texture DX11::CreateCubemap(byte const * const Data, Math::Vector2u const & Size)
		{
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

			return Index;
		}

		RenderingBackend::MeshProxy DX11::CreateMeshProxy(Funky::RawMesh const * Mesh)
		{
			DX11::DX11GPUMesh Returner;

			D3D11_BUFFER_DESC VertexBufferDesc;
			VertexBufferDesc.ByteWidth = (u32)(GetVertices(Mesh).size() * sizeof(Vertex));
			VertexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			VertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			VertexBufferDesc.CPUAccessFlags = 0; // D3D11_CPU_ACCESS_FLAG 
			VertexBufferDesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_FLAG 
			VertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA VertexData;
			ZeroMemory(&VertexData, sizeof(D3D11_SUBRESOURCE_DATA));
			VertexData.pSysMem = GetVertices(Mesh).data();
			ASSERT(VertexData.SysMemPitch == 0, L"memory was not ZEROED");

			HRESULT hr1 = pDevice->CreateBuffer(&VertexBufferDesc, &VertexData, Returner.pVertexBuffer.GetAddressOf());
			ASSERT(SUCCEEDED(hr1), L" Vertex buffer was not created properly!");

			D3D11_BUFFER_DESC IndexBufferDesc;
			IndexBufferDesc.ByteWidth = (u32)(GetIndices(Mesh).size() * sizeof(u16));
			IndexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
				IndexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
				IndexBufferDesc.CPUAccessFlags = 0; // D3D11_CPU_ACCESS_FLAG 
			IndexBufferDesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_FLAG 
			IndexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA IndexData;
			ZeroMemory(&IndexData, sizeof(D3D11_SUBRESOURCE_DATA));
			IndexData.pSysMem = GetIndices(Mesh).data();
			ASSERT(IndexData.SysMemPitch == 0, L"memory was not ZEROED");

			HRESULT hr2 = pDevice->CreateBuffer(&IndexBufferDesc, &IndexData, Returner.pIndexBuffer.GetAddressOf());
			ASSERT(SUCCEEDED(hr2), TEXT(" Index buffer was not created properly!"));

			if (hr1 == S_OK && hr2 == S_OK)
			{
				Returner.IndicesCount = Mesh->GetIndicesCount();

				Meshes.push_back(Move(Returner));
				return Meshes.size() - 1;
			}

			return RenderingBackend::INVALID_INDEX;
		}

		void DX11::BindRenderTarget(RenderingBackend::RenderTarget RenderTargetToBind)
		{
			//TODO(ekicam2): it's hacky to unbind it all the time, render target should cache is somehow
			ID3D11ShaderResourceView* rt1[] = { nullptr, nullptr, nullptr };
			pDeviceContext->PSSetShaderResources(0, 3u, rt1);
			pDeviceContext->VSSetShaderResources(0, 3u, rt1);

			ID3D11RenderTargetView* rt[] = { RenderTargets[RenderTargetToBind].pRenderTargetView.Get() };
			//TODO(ekicam2): should we bind here defult depthstencil?
			pDeviceContext->OMSetRenderTargets(1, rt, pDepthStencilView.Get());
		}

		void DX11::BindDefaultRenderTarget()
		{
			// CComPtr<ID3DUserDefinedAnnotation> pPerf;
			// HRESULT hr = pDeviceContext->QueryInterface(__uuidof(pPerf), reinterpret_cast<void**>(&pPerf));
			// if (SUCCEEDED(hr))
			// 	pPerf->BeginEvent(L"Now entering ocean rendering code");
			// 
			// if (SUCCEEDED(hr))
			// 	pPerf->EndEvent();

			ID3D11RenderTargetView* rt[] = { pRenderTarget.Get() };
			pDeviceContext->OMSetRenderTargets(1, rt, pDepthStencilView.Get());
		}

		void DX11::ClearRenderTargetWithColor(Math::Vector3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear)
		{
			float color[4] = { Color.X, Color.Y, Color.Z, 1.0f };


			ID3D11RenderTargetView* FinalTarget = RenderTargetToClear == RenderingBackend::INVALID_INDEX ? pRenderTarget.Get() : RenderTargets[RenderTargetToClear].pRenderTargetView.Get();
			pDeviceContext->ClearRenderTargetView(FinalTarget, color);
		}

		void DX11::ClearDepthStencil(float Depth, float Stencil)
		{
			pDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, Depth, (u8)Stencil);
		}

		void DX11::SetPrimitiveTopology(RenderingBackend::PrimitiveTopology NewTopology)
		{
			pDeviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)NewTopology);
		}

		void DX11::UpdateConstantBuffer(RenderingBackend::ConstantBuffer ConstantBuffer, RenderingBackend::ConstantBufferData Data)
		{
			D3D11_MAPPED_SUBRESOURCE MappedData;
			ZeroMemory(&MappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));

			pDeviceContext->Map(ConstantBuffers[ConstantBuffer].pConstantBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedData);
			memcpy(MappedData.pData, Data, ConstantBuffers[ConstantBuffer].SizeInBytes);
			pDeviceContext->Unmap(ConstantBuffers[ConstantBuffer].pConstantBuffer.Get(), 0);
		}

		void DX11::BindConstantBuffer(RenderingBackend::ShaderResourceStage Stage, Rendering::RenderingBackend::ConstantBuffer const & Buffers, u32 StartIndex)
		{
			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetConstantBuffers(StartIndex, 1u, (ID3D11Buffer * const *)ConstantBuffers[Buffers].pConstantBuffer.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetConstantBuffers(StartIndex, 1u, (ID3D11Buffer * const *)ConstantBuffers[Buffers].pConstantBuffer.GetAddressOf());
				break;
			}
		}

		void DX11::BindVertexShader(RenderingBackend::VertexShader VertexShaderToBind)
		{
			pDeviceContext->IASetInputLayout(VertexShaders[VertexShaderToBind].pInputLayout.Get());
			pDeviceContext->VSSetShader(VertexShaders[VertexShaderToBind].pVertexShader.Get(), nullptr, 0);
		}

		void DX11::BindPixelShader(RenderingBackend::PixelShader PixelShaderToBind)
		{
			pDeviceContext->PSSetShader(PixelShaders[PixelShaderToBind].pPixelShader.Get(), nullptr, 0);
		}

		void DX11::DrawMesh(RenderingBackend::MeshProxy Mesh)
		{
			u32 stride = sizeof(Vertex), offset = 0;
			pDeviceContext->IASetVertexBuffers(0u, 1u, Meshes[Mesh].pVertexBuffer.GetAddressOf(), &stride, &offset);
			pDeviceContext->IASetIndexBuffer(Meshes[Mesh].pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
			pDeviceContext->DrawIndexed((u32)Meshes[Mesh].IndicesCount, 0, 0);
		}

		void DX11::Present()
		{
			pSwapChain->Present(1, 0);
		}


		bool DX11::CreateDeviceAndSwapchain(HWND hwnd)
		{
			D3D_FEATURE_LEVEL pFeatureLevels[] = {
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_11_1
			};

			WindowHandle = hwnd;

			RECT ClientSize;
			GetClientRect(hwnd, &ClientSize);

			DXGI_SWAP_CHAIN_DESC SwapChainDesc;
			ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

			SwapChainDesc.Windowed = TRUE;
			SwapChainDesc.SampleDesc.Count = 1u;
			SwapChainDesc.SampleDesc.Quality = 0u;
			SwapChainDesc.BufferCount = 2u;
			SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDesc.BufferDesc.Width = ClientSize.right - ClientSize.left;
			SwapChainDesc.BufferDesc.Height = ClientSize.bottom - ClientSize.top;
			SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1u;
			SwapChainDesc.BufferDesc.RefreshRate.Denominator = 75u;
			SwapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
			SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE; // dk
			SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_CENTERED;
			SwapChainDesc.OutputWindow = hwnd;
			SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			SwapChainDesc.Flags;

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

			LOG_FUNKY(TEXT("actually supported level is: "), DirectUtils::FeatureNameToString(ActuallySupportedLevel));


			Microsoft::WRL::ComPtr<IDXGIDevice> Device;
			pDevice.As(&Device);

			Microsoft::WRL::ComPtr<IDXGIAdapter> Adapter;
			Device->GetAdapter(Adapter.GetAddressOf());

			Microsoft::WRL::ComPtr<IDXGIFactory> Factory;
			hr = Adapter->GetParent(IID_PPV_ARGS(&Factory));
			ASSERT(SUCCEEDED(hr), TEXT("couldn't create swapchain"));

			if (SUCCEEDED(hr))
			{
				hr = Factory->CreateSwapChain(pDevice.Get(), &SwapChainDesc, pSwapChain.GetAddressOf());
				ASSERT(SUCCEEDED(hr), TEXT("couldn't create swapchain"));
			}

			return true;
		}

		bool DX11::InitSwapchain()
		{
			HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("couldn't get back buffer"));
				return false;
			}

			D3D11_TEXTURE2D_DESC BackBufferDesc;
			pBackBuffer->GetDesc(&BackBufferDesc);

			// D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
			// ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			// DepthStencilDesc.DepthEnable = TRUE;
			// DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

			D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
			ZeroMemory(&DepthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilTextureDesc.Height = BackBufferDesc.Height;
			DepthStencilTextureDesc.Width = BackBufferDesc.Width;
			DepthStencilTextureDesc.ArraySize = 1;
			DepthStencilTextureDesc.MipLevels = 1;
			DepthStencilTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
			DepthStencilTextureDesc.SampleDesc.Count = 1;

			hr = pDevice->CreateTexture2D(&DepthStencilTextureDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("Couldn't create depth stencil buffer"));
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
			ZeroMemory(&DepthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			DepthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;

			pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), &DepthViewDesc, pDepthStencilView.GetAddressOf());

			static const float aspectRatio = static_cast<float>(BackBufferDesc.Width) / static_cast<float>(BackBufferDesc.Height);

			D3D11_VIEWPORT Viewport;
			ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
			Viewport.Height = static_cast<float>(BackBufferDesc.Height);
			Viewport.Width = static_cast<float>(BackBufferDesc.Width);
			Viewport.MinDepth = 0.0f;
			Viewport.MaxDepth = 1.0f;

			pDeviceContext->RSSetViewports(1, &Viewport);

			hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTarget.GetAddressOf());
			if (!SUCCEEDED(hr))
			{
				LOG_ERROR(TEXT("couldn't create render target"));
				return false;
			}

			return true;
		}

		void DX11::FreeSwapchain()
		{
#define FK_COM_SAFERELEASE(x) if(x) x->Release();

			FK_COM_SAFERELEASE(pRenderTarget);
			FK_COM_SAFERELEASE(pDepthStencilView);
		}

		void DX11::BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, u32 StartIndex /*= 0u*/)
		{
			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetShaderResources(StartIndex, 1u, Textures[Texture].pTextureView.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetShaderResources(StartIndex, 1u, Textures[Texture].pTextureView.GetAddressOf());
				break;
			}
		}

		void DX11::BindTexture(RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, u32 StartIndex /*= 0u*/)
		{
			switch (Stage)
			{
			case RenderingBackend::ShaderResourceStage::Fragment:
				pDeviceContext->PSSetShaderResources(StartIndex, 1u, RenderTargets[Texture].pTextureView.GetAddressOf());
				break;
			case RenderingBackend::ShaderResourceStage::Vertex:
				pDeviceContext->VSSetShaderResources(StartIndex, 1u, RenderTargets[Texture].pTextureView.GetAddressOf());
				break;
			}
		}
	}
}