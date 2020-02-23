#include "Renderer.h"
#include "RenderingResourceManager.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib,"d3dcompiler.lib")

bool Funky::Rendering::Renderer::Init()
{
	RT = RenderingBackend.GetResourceManager()->GetSwapchainRenderTarget();
	DS = RenderingBackend.GetResourceManager()->GetSwapchainDepthStencil();

	Math::Vec3f vertices[] = {
		{ 0.0f, .25f, 1.0f },
		{ 0.25f, 0.0f, 1.0f },
		{ -0.25f, 0.0f, 1.0f }
	};
	static_assert(sizeof(vertices) == 3 * sizeof(Math::Vec3f));

	u16 indices[] = { 0, 1, 2 };
	static_assert(sizeof(indices) == 3 * sizeof(u16));

	vb = RenderingBackend.CreateBuffer(sizeof(vertices), RBuffer::Type::Vertex, RBuffer::UsageType::Static, vertices);
	ib = RenderingBackend.CreateBuffer(sizeof(indices), RBuffer::Type::Index, RBuffer::UsageType::Static, indices);


	{
		const char* vs = R"(
			float4 VSMain(float3 vPosition : POSITION) : SV_POSITION
			{	
				return float4(vPosition, 1.0f);
			}
		)";

		D3D_SHADER_MACRO Macros[] = { nullptr };
		Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
		Microsoft::WRL::ComPtr <ID3DBlob> Errors;

		HRESULT hr = D3DCompile(vs, strlen(vs), NULL, Macros, NULL, "VSMain", "vs_5_0", 0, 0, CodeBlob.GetAddressOf(), Errors.GetAddressOf());
		if (!SUCCEEDED(hr))
		{
			[[maybe_unused]] const char* ErrorStr = (const char*)Errors->GetBufferPointer();
			[[maybe_unused]] size ErrorLength = Errors->GetBufferSize();

			CHECK(SUCCEEDED(hr));
		}

		RenderingBackend::ShaderInputDesc ShaderDesc;

		ShaderDesc.ShaderData = (byte*)CodeBlob->GetBufferPointer();
		ShaderDesc.DataSize = CodeBlob->GetBufferSize();

		Shaders.VS = RenderingBackend.CreateVertexShader(&ShaderDesc);
	}

	{
		const char* ps = R"(
			float4 PSMain(float4 vPosition : SV_POSITION) : SV_TARGET0 
			{	
				return float4(1.0f, 0.0f, 1.0f, 1.0f);
			}
		)";

		D3D_SHADER_MACRO Macros[] = { nullptr };
		Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
		Microsoft::WRL::ComPtr <ID3DBlob> Errors;

		HRESULT hr = D3DCompile(ps, strlen(ps), NULL, Macros, NULL, "PSMain", "ps_5_0", 0, 0, CodeBlob.GetAddressOf(), Errors.GetAddressOf());
		if (!SUCCEEDED(hr))
		{
			[[maybe_unused]] const char* ErrorStr = (const char*)Errors->GetBufferPointer();
			[[maybe_unused]] size ErrorLength = Errors->GetBufferSize();

			CHECK(SUCCEEDED(hr));
		}

		RenderingBackend::ShaderInputDesc ShaderDesc;

		ShaderDesc.ShaderData = (byte*)CodeBlob->GetBufferPointer();
		ShaderDesc.DataSize = CodeBlob->GetBufferSize();

		Shaders.PS = RenderingBackend.CreatePixelShader(&ShaderDesc);
	}

	return true;
}

void Funky::Rendering::Renderer::Shutdown()
{

}

void Funky::Rendering::Renderer::DrawScene(class RenderScene* SceneToRender)
{
	(void*)SceneToRender;

	RenderingBackend.BindRenderTarget(RT);
	RenderingBackend.ClearRenderTarget(RT, { 0.392156899f, 0.584313750f, 0.929411829f });
	RenderingBackend.ClearDepthStencil(DS, 0.0f, 0u);
	RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);

	RenderingBackend.BindVertexShader(Shaders.VS);
	RenderingBackend.BindPixelShader(Shaders.PS);

	RenderingBackend.Draw(vb);


	RenderingBackend.Present();
}
