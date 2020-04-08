#include "Renderer.h"
#include "RenderingResourceManager.h"
#include "UserFramework/Scene.h"

#include "ShaderCompiler.h"
#include "Core/Platform/Platform.h"
#include "RenderingBackend/Marker.h"

#include <DirectXMath.h>


Math::Vec3f cmapos = Math::Vec::FORWARD * 14.0f;

bool Funky::Rendering::Renderer::Init()
{
	PerViewConstantBufferHandle = RenderingBackend.CreateBuffer(
		sizeof(PerViewConstantBuffer),
		Rendering::RBuffer::EType::Uniform,
		Rendering::RBuffer::EUsageType::Dynamic,
		&PerViewConstantBufferData
	);

	PerObjectConstantBufferHandle = RenderingBackend.CreateBuffer(
		sizeof(PerObjectConstantBuffer),
		Rendering::RBuffer::EType::Uniform,
		Rendering::RBuffer::EUsageType::Dynamic,
		&PerObjectConstantBufferData
	);
	
	
	OffscreenRT = RenderingBackend.CreateRenderTarget(RenderingBackend.GetResourceManager()->GetSwapchainRenderTarget()->Size);

	auto VSSource = Platform::ReadFile("RealData/Shaders/Source/PPVS.hlsl");
	auto PSSource = Platform::ReadFile("RealData/Shaders/Source/ToonPP.hlsl");

	auto VS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Vertex, VSSource);
	auto PS = Asset::Shader::CreateShaderFromSource(Asset::Shader::EShaderType::Pixel, PSSource);
	PPMaterial.Reset(Asset::Material::CreateMaterial(VS, PS));

	ShaderCompiler::ShaderDesc Desc;
	Desc.Api = RenderingBackend.GetBackendAPI();
	Desc.EntryPoint = "VSMain";
	Desc.ShaderAsset = VS;
	ShaderCompiler::CompileShader(Desc);

	Desc.EntryPoint = "PSMain";
	Desc.ShaderAsset = PS;
	ShaderCompiler::CompileShader(Desc);

	PPMaterial->Linkage.VS = [&]() -> Rendering::RShader* {

		RenderingBackend::ShaderInputDesc ShaderDesc;
		ShaderDesc.ShaderData = PPMaterial->GetVS()->GetBuffer();
		ShaderDesc.DataSize = PPMaterial->GetVS()->GetBufferSize();

		return RenderingBackend.CreateVertexShader(&ShaderDesc);
	}();

	PPMaterial->Linkage.PS = [&]() -> Rendering::RShader* {


		RenderingBackend::ShaderInputDesc ShaderDesc;
		ShaderDesc.ShaderData = PPMaterial->GetPS()->GetBuffer();
		ShaderDesc.DataSize = PPMaterial->GetPS()->GetBufferSize();

		return RenderingBackend.CreatePixelShader(&ShaderDesc);
	}();

	PostProcess::InitPostProcessSurface(&RenderingBackend);


	return true;
}

void Funky::Rendering::Renderer::Shutdown()
{

}

Funky::Rendering::RenderView* Funky::Rendering::Renderer::CreateRenderScene(IScene* InScene)
{
	VisibleObject* SceneObjects;
	const i32 SceneObjectsNum = InScene->GetVisibleObjects(SceneObjects);

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

		CHECK(SceneObject->Material->IsValid());

		if (!SceneObject->Material->Linkage.VS)
		{
			SceneObject->Material->Linkage.VS = [&]() -> Rendering::RShader* {

				auto VS = SceneObject->Material->GetVS();

				if (!VS->IsValid())
				{
					ShaderCompiler::ShaderDesc Desc;
					Desc.Api = Rendering::RenderingBackend::EAPI::DX11;
					Desc.EntryPoint = "VSMain";
					Desc.ShaderAsset = VS;
					ShaderCompiler::CompileShader(Desc);
				}

				RenderingBackend::ShaderInputDesc ShaderDesc;
				ShaderDesc.ShaderData = VS->GetBuffer();
				ShaderDesc.DataSize = SceneObject->Material->GetVS()->GetBufferSize();

				return RenderingBackend.CreateVertexShader(&ShaderDesc);
			}();
		}
		
		if (!SceneObject->Material->Linkage.PS)
		{
			SceneObject->Material->Linkage.PS = [&]() -> Rendering::RShader* {

				auto PS = SceneObject->Material->GetPS();
				if (!PS->IsValid())
				{
					ShaderCompiler::ShaderDesc Desc;
					Desc.EntryPoint = "PSMain";
					Desc.ShaderAsset = PS;
					ShaderCompiler::CompileShader(Desc);
				}

				RenderingBackend::ShaderInputDesc ShaderDesc;
				ShaderDesc.ShaderData = PS->GetBuffer();
				ShaderDesc.DataSize = PS->GetBufferSize();

 				return RenderingBackend.CreatePixelShader(&ShaderDesc);
			}();
		}

		CHECK(SceneObject->Material->Linkage.VS);
		CHECK(SceneObject->Material->Linkage.PS);

	}

	
	RenderView* Ret = new RenderView();
	Ret->Camera = InScene->GetCamera();

	for (u8 i = 0; i < SceneObjectsNum && i < MAX_RENDER_PRIMITIVES; ++i)
	{
		VisibleObject* const SceneObject = &SceneObjects[i];

		Ret->Objects[i].bIsValid = true;

		Ret->Objects[i].Mesh.VertexBuffer = SceneObject->Mesh->VertexBuffer;
		Ret->Objects[i].Mesh.IndexBuffer  = SceneObject->Mesh->IndexBuffer;

		Ret->Objects[i].Shaders = SceneObject->Material->Linkage;

		Ret->Objects[i].Position = SceneObject->Position;
		Ret->Objects[i].Rotation = SceneObject->Rotation;
	}

	return Ret;
}

void Funky::Rendering::Renderer::DrawScene(RenderView* SceneToRender)
{
#define GPU_MARKER(Name) \
	GPUScopeMarker CONCAT(CONCAT(Marker, __LINE__),(&RenderingBackend, Name))

	auto DefaultDS = RenderingBackend.GetResourceManager()->GetSwapchainDepthStencil();
	{
		GPU_MARKER("Begin Scene");
		RenderingBackend.BindRenderTarget(OffscreenRT, DefaultDS);
		RenderingBackend.ClearRenderTarget(OffscreenRT, { 0.392156899f, 0.584313750f, 0.929411829f });
		RenderingBackend.ClearDepthStencil(DefaultDS, 1.0f, 0u);
		RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
		//DirectX::XMMatrixLookAtLH()
	}

	{
		GPU_MARKER("MainPass");
		for (u8 i = 0; i < MAX_RENDER_PRIMITIVES; ++i)
		{
			if(!SceneToRender->Objects[i].bIsValid) continue;

			RenderPrimitive* CurrentObject = &SceneToRender->Objects[0];

			CHECK(nullptr != CurrentObject->Mesh.VertexBuffer);
			CHECK(nullptr != CurrentObject->Mesh.IndexBuffer);

			//Math::Mat4f Model = Math::Mat4f::Identity;
			DirectX::XMMATRIX Model = DirectX::XMMatrixRotationRollPitchYaw(
				Math::ToRad(CurrentObject->Rotation.X), 
				Math::ToRad(CurrentObject->Rotation.Y), 
				Math::ToRad(CurrentObject->Rotation.Z)
			);
			Model *= DirectX::XMMatrixTranslation(CurrentObject->Position.X, CurrentObject->Position.Y, CurrentObject->Position.Z);

			auto VP = SceneToRender->Camera->GetView() * SceneToRender->Camera->GetProjection();

			PerViewConstantBufferData.VP			 = VP;
			PerViewConstantBufferData.CameraPosition = Math::Vec4f(SceneToRender->Camera->GetPosition(), 1.0f);
			PerViewConstantBufferData.CameraForward	 = Math::Vec4f(SceneToRender->Camera->GetForward(), 0.0f);
			RenderingBackend.BindConstantBuffer(RenderingBackend::ShaderResourceStage::Fragment, PerViewConstantBufferHandle, 0);
			RenderingBackend.UpdateConstantBuffer(PerViewConstantBufferHandle, &PerViewConstantBufferData);

			PerObjectConstantBufferData.MVP	  = Model * VP;
			PerObjectConstantBufferData.Model = Model;
			RenderingBackend.BindConstantBuffer(RenderingBackend::ShaderResourceStage::Vertex, PerObjectConstantBufferHandle, 0);
			RenderingBackend.UpdateConstantBuffer(PerObjectConstantBufferHandle, &PerObjectConstantBufferData);

			RenderingBackend.BindVertexShader(CurrentObject->Shaders.VS);
			RenderingBackend.BindPixelShader(CurrentObject->Shaders.PS);

			auto& Mesh = CurrentObject->Mesh;
			if (Mesh.IndexBuffer->ElementCount)
				RenderingBackend.DrawIndexed(
					Mesh.VertexBuffer,
					Mesh.IndexBuffer
				);
			else
				RenderingBackend.Draw(Mesh.VertexBuffer);

		}
	}


	// Post Process
	//
	{
		GPU_MARKER("Post Process");

		auto DefaultRT = RenderingBackend.GetResourceManager()->GetSwapchainRenderTarget();
		RenderingBackend.BindRenderTarget(DefaultRT);

		RenderingBackend.BindTexture(RenderingBackend::ShaderResourceStage::Fragment, OffscreenRT, 0);
		RenderingBackend.BindTexture(RenderingBackend::ShaderResourceStage::Fragment, DefaultDS,   1);

		RenderingBackend.BindVertexShader(PPMaterial->Linkage.VS);
		RenderingBackend.BindPixelShader(PPMaterial->Linkage.PS);
		RenderingBackend.Draw(PostProcess::GetPostProcessSurface());

	
		RenderingBackend.Present();
	}
}

