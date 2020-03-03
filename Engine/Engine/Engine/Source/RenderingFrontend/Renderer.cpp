#include "Renderer.h"
#include "RenderingResourceManager.h"
#include "UserFramework/Scene.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include <DirectXMath.h>

#pragma comment(lib,"d3dcompiler.lib")

Math::Vec3f cmapos = { 0.0f, 0.0f, 0.0f };

bool Funky::Rendering::Renderer::Init()
{
	ConstantBuffer = RenderingBackend.CreateBuffer(
		sizeof(BaseConstantBuffer),
		Rendering::RBuffer::EType::Uniform,
		Rendering::RBuffer::EUsageType::Dynamic,
		&ConstantBufferData);

	return true;
}

void Funky::Rendering::Renderer::Shutdown()
{

}

Funky::Rendering::RenderScene* Funky::Rendering::Renderer::CreateRenderScene(IScene* Scene)
{
	VisibleObject* SceneObjects;
	const i32 SceneObjectsNum = Scene->GetVisibleObjects(SceneObjects);

	for (u8 i = 0; (i < SceneObjectsNum); ++i)
	{
		VisibleObject * const SceneObject = &SceneObjects[i];

		
		if(!SceneObject->Mesh->VertexBuffer)
			SceneObject->Mesh->VertexBuffer = RenderingBackend.CreateBuffer(
				SceneObject->Mesh->GetVertexBufferSizeInBytes(),
				RBuffer::EType::Vertex, 
				RBuffer::EUsageType::Static, 
				SceneObject->Mesh->GetVertices()
			);

		if(!SceneObject->Mesh->IndexBuffer)
			SceneObject->Mesh->IndexBuffer  = RenderingBackend.CreateBuffer(
				SceneObject->Mesh->GetIndexBufferSizeInBytes(),
				RBuffer::EType::Index,
				RBuffer::EUsageType::Static,
				SceneObject->Mesh->GetIndices()
			);

		CHECK(SceneObject->Mesh->VertexBuffer);
		CHECK(SceneObject->Mesh->IndexBuffer);


		if (!SceneObject->Material->Linkage.VS)
		{
			SceneObject->Material->Linkage.VS = [&]() {

				D3D_SHADER_MACRO Macros[] = { nullptr };
				Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
				Microsoft::WRL::ComPtr <ID3DBlob> Errors;

				const char* vs = R"(
			cbuffer ModelViewProjectionConstantBuffer : register(b0)
			{
				// REMEMBER IN HLSL matrices are column major
				matrix mvp;
				matrix model;
				float3 lookat;
				float padding;
			};

			float4 VSMain(float3 vPosition : POSITION, float3 vColor : COLOR, float3 vNormal : NORMAL, float2 vUv : UV) : SV_POSITION
			{	
				matrix cosglupiego = model * mvp;
				float4 Pos = mul(mvp, float4(vPosition, 1.0));
				return Pos;//float4(vPosition, 1.0);
			}
		)";

				HRESULT hr = D3DCompile(
					//SceneObject->Material->GetVertexShaderSourceCode(), 
					//SceneObject->Material->GetVertexShaderSourceCodeLength(),
					vs,
					strlen(vs),
					NULL,
					Macros,
					NULL,
					"VSMain",
					"vs_5_0",
					0,
					0,
					CodeBlob.GetAddressOf(),
					Errors.GetAddressOf()
				);
				if (!SUCCEEDED(hr))
				{
					const char* ErrorStr = (const char*)Errors->GetBufferPointer();
					LOG_ERROR(ErrorStr);

					CHECK(SUCCEEDED(hr));
				}

				RenderingBackend::ShaderInputDesc ShaderDesc;

				ShaderDesc.ShaderData = (byte*)CodeBlob->GetBufferPointer();
				ShaderDesc.DataSize = CodeBlob->GetBufferSize();

				return RenderingBackend.CreateVertexShader(&ShaderDesc);
			}();
		}
		
		if (!SceneObject->Material->Linkage.PS)
		{
			SceneObject->Material->Linkage.PS = [&]() {


				D3D_SHADER_MACRO Macros[] = { nullptr };
				Microsoft::WRL::ComPtr <ID3DBlob> CodeBlob;
				Microsoft::WRL::ComPtr <ID3DBlob> Errors;

				const char* ps = R"(
			float4 PSMain(float4 vPosition : SV_POSITION) : SV_TARGET0 
			{	
				return float4(1.0f, 0.0f, 1.0f, 1.0f);
			}
		)";

				HRESULT hr = D3DCompile(
					//SceneObject->Material->GetPixelShaderSourceCode(),
					//SceneObject->Material->GetPixelShaderSourceCodeLength(),
					ps,
					strlen(ps),
					NULL, 
					Macros, 
					NULL, 
					"PSMain", 
					"ps_5_0", 
					0, 
					0, 
					CodeBlob.GetAddressOf(), 
					Errors.GetAddressOf()
				);
				if (!SUCCEEDED(hr))
				{
					const char* ErrorStr = (const char*)Errors->GetBufferPointer();
					LOG_ERROR(ErrorStr);

					CHECK(SUCCEEDED(hr));
				}

				RenderingBackend::ShaderInputDesc ShaderDesc;

				ShaderDesc.ShaderData = (byte*)CodeBlob->GetBufferPointer();
				ShaderDesc.DataSize = CodeBlob->GetBufferSize();

				return RenderingBackend.CreatePixelShader(&ShaderDesc);
			}();
		}

		CHECK(SceneObject->Material->Linkage.VS);
		CHECK(SceneObject->Material->Linkage.PS);

	}

	
	RenderScene* Ret = new RenderScene();
	for (u8 i = 0; i < SceneObjectsNum && i < MAX_RENDER_PRIMITIVES; ++i)
	{
		VisibleObject* const SceneObject = &SceneObjects[i];

		Ret->Objects[i].bIsValid = true;

		Ret->Objects[i].Mesh.VertexBuffer = SceneObject->Mesh->VertexBuffer;
		Ret->Objects[i].Mesh.IndexBuffer  = SceneObject->Mesh->IndexBuffer;

		Ret->Objects[i].Shaders = SceneObject->Material->Linkage;

		Ret->Objects[i].Position = SceneObject->Position;
	}

	return Ret;
}

void Funky::Rendering::Renderer::DrawScene(RenderScene* SceneToRender)
{

	auto RT = RenderingBackend.GetResourceManager()->GetSwapchainRenderTarget();
	auto DS = RenderingBackend.GetResourceManager()->GetSwapchainDepthStencil();
	RenderingBackend.BindRenderTarget(RT, nullptr);
	RenderingBackend.ClearRenderTarget(RT, { 0.392156899f, 0.584313750f, 0.929411829f });
	RenderingBackend.ClearDepthStencil(DS, 0.0f, 0u);
	RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
	//DirectX::XMMatrixLookAtLH()


	for (u8 i = 0; i < MAX_RENDER_PRIMITIVES; ++i)
	{
		if(!SceneToRender->Objects[i].bIsValid) continue;

		RenderPrimitive* CurrentObject = &SceneToRender->Objects[i];

		CHECK(nullptr != CurrentObject->Mesh.VertexBuffer);
		CHECK(nullptr != CurrentObject->Mesh.IndexBuffer);


		Math::Mat4f Model = Math::Mat4f::Identity;
		Math::Mat4f::Translate(Model, CurrentObject->Position);

		auto View = Math::Mat4f::Identity;
		View = Math::Mat4f::LookAtLH(cmapos, Math::Vec::FORWARD * 200.0f, Math::Vec::UP);
		
		//View = Math::Mat4f::Identity;

		auto v = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 10.0, 0.0, 100.0f }, { 0.0f, 1.0f, 0.0 });
		auto v1 = DirectX::XMMatrixLookAtRH({ 0.0f, 0.0f, 0.0f }, { 10.0, 0.0, 100.0f }, { 0.0f, 1.0f, 0.0 });

		auto Proj = Math::Mat4f::ProjectionMatrixLH((float)(2048u) / 1024u, 90.0f, 1.f, 100.f);

		ConstantBufferData.MVP = Model * View * Proj;// Math::Mat4f::Identity;
		

		ConstantBufferData.LookAt = { 1.0, 2.0 ,3.0 };
		ConstantBufferData.__padding = 24.0f;

		RenderingBackend.BindConstantBuffer(RenderingBackend::ShaderResourceStage::Vertex, ConstantBuffer);
		RenderingBackend.UpdateConstantBuffer(ConstantBuffer, &ConstantBufferData);

		RenderingBackend.BindVertexShader(CurrentObject->Shaders.VS);
		RenderingBackend.BindPixelShader(CurrentObject->Shaders.PS);

		auto& Mesh = CurrentObject->Mesh;
		//if (Mesh.IndexBuffer->ElementCount)
		//	RenderingBackend.DrawIndexed(
		//		Mesh.VertexBuffer,
		//		Mesh.IndexBuffer
		//	);
		//else
			RenderingBackend.Draw(Mesh.VertexBuffer);

	}

	
	
	
	RenderingBackend.Present();
}

