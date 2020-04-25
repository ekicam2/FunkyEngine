#include "Core/Events/EngineEvents.h"
#include "Engine.h"

#include "Renderer.h"
#include "RenderingResourceManager.h"
#include "UserFramework/Scene.h"

#include "Core/Timer.h"

#include "ShaderCompiler.h"
#include "Core/Platform/Platform.h"
#include "RenderingBackend/Marker.h"

#include <DirectXMath.h>


#define GPU_MARKER(Name) GPUScopeMarker CONCAT(Marker,__LINE__)(&RenderingBackend, Name)

Math::Vec3f cmapos = Math::Vec::FORWARD * 14.0f;

bool Funky::Rendering::Renderer::Init(Rendering::RenderingBackend::RenderingBackendInitResult* renderingBackendInitResult)
{
	RRManager.Reset(new RenderingResourcesManager());
	RRManager->AssociateSwapchain(renderingBackendInitResult->SwapchainRT, renderingBackendInitResult->SwapchainDS);

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
	
	OffscreenRT = RRManager->CreateResource<Rendering::RRenderTarget>(RenderingBackend.CreateRenderTarget(Funky::Engine::GetEngine()->GetWindowSize()));
	
	Funky::OnViewportResized.RegisterLambda([this] (Math::Vec2u NewSize){
		RRManager->FreeResource<Rendering::RRenderTarget>(OffscreenRT);
		OffscreenRT = RRManager->CreateResource<Rendering::RRenderTarget>(RenderingBackend.CreateRenderTarget(NewSize));
	});

	PostProcess::InitPostProcessSurface(&RenderingBackend);
	PostProcess::InitVS(&RenderingBackend);

	PP.Reset(PostProcess::CreateFromSource("RealData/Shaders/Source/ToonPP.hlsl", &RenderingBackend));

	return true;
}

void Funky::Rendering::Renderer::Shutdown()
{
	RRManager->FreeAll();
}

Funky::Rendering::RenderView* Funky::Rendering::Renderer::CreateRenderScene([[maybe_unused]]IScene* InScene)
{
	auto SceneObjects = InScene->GetVisibleObjects();
	size SceneObjectsNum = SceneObjects.size();

	RenderView* Ret = new RenderView();
	Ret->Camera = InScene->GetCamera();

	for (size i = 0; (i < SceneObjectsNum && i < MAX_RENDER_PRIMITIVES); ++i)
	{
		VisibleObject * const sceneObject = &SceneObjects[i];

		auto PrepareMeshFor = [=](VisibleObject* const sceneObject) -> Asset::StaticMesh*
		{
			auto& ar = AssetRegistry::GetInstance();
			auto mesh = ar.GetAsset<Asset::StaticMesh>(sceneObject->Mesh);
			{
				if (mesh->VertexBuffer == Rendering::Resource::ID::Zero)
					mesh->VertexBuffer = RRManager->CreateResource<Rendering::RBuffer>(
						RenderingBackend.CreateBuffer(
							mesh->GetVertexBufferSizeInBytes(),
							RBuffer::EType::Vertex,
							RBuffer::EUsageType::Static,
							mesh->GetVertices()
						)
						);


				if (mesh->GetIndicesCount() > 0 && mesh->IndexBuffer == Rendering::Resource::ID::Zero)
					mesh->IndexBuffer = RRManager->CreateResource<Rendering::RBuffer>(
						RenderingBackend.CreateBuffer(
							mesh->GetIndexBufferSizeInBytes(),
							RBuffer::EType::Index,
							RBuffer::EUsageType::Static,
							mesh->GetIndices()
						)
						);
			}
			CHECK(mesh->VertexBuffer != Rendering::Resource::ID::Zero);
			CHECK(mesh->GetIndicesCount() == 0 || (mesh->GetIndicesCount() > 0 && mesh->IndexBuffer != Rendering::Resource::ID::Zero));
			
			return mesh;
		};
		auto mesh = PrepareMeshFor(sceneObject);

		struct ShadersRef { Asset::Shader* vs; Asset::Shader* ps; };
		auto PrepareMaterial = [=](VisibleObject* const sceneObject) -> ShadersRef
		{
			auto& ar = AssetRegistry::GetInstance();
			auto material = ar.GetAsset<Asset::Material>(sceneObject->Material);
			auto vs = ar.GetAsset<Asset::Shader>(material->VS);
			auto ps = ar.GetAsset<Asset::Shader>(material->PS);

			CHECK(vs->IsValid() && vs->IsCompiled());
			CHECK(ps->IsValid() && ps->IsCompiled());


			if (vs->ShaderHandle == Rendering::Resource::ID::Zero)
			{
				RenderingBackend::ShaderInputDesc ShaderCreationInfo;
				ShaderCreationInfo.ShaderData = vs->GetBuffer();
				ShaderCreationInfo.DataSize = vs->GetBufferSize();

				vs->ShaderHandle = RRManager->CreateResource<Rendering::RShader>(
					RenderingBackend.CreateVertexShader(&ShaderCreationInfo)
					);
			}

			if (ps->ShaderHandle == Rendering::Resource::ID::Zero)
			{
				RenderingBackend::ShaderInputDesc ShaderCreationInfo;
				ShaderCreationInfo.ShaderData = ps->GetBuffer();
				ShaderCreationInfo.DataSize = ps->GetBufferSize();

				ps->ShaderHandle = RRManager->CreateResource<Rendering::RShader>(
					RenderingBackend.CreatePixelShader(&ShaderCreationInfo)
				);
			}

			return ShadersRef{ vs, ps };
		};
		auto [vs, ps] = PrepareMaterial(sceneObject);

		Ret->Objects[i].bIsValid = true;

		Ret->Objects[i].Mesh.VertexBuffer = RRManager->GetResource<Rendering::RBuffer>(mesh->VertexBuffer);
		Ret->Objects[i].Mesh.IndexBuffer  = mesh->GetIndicesCount() > 0u ? RRManager->GetResource<Rendering::RBuffer>(mesh->IndexBuffer) : nullptr;

		Ret->Objects[i].Shaders.VS = RRManager->GetResource<Rendering::RShader>(vs->ShaderHandle);
		Ret->Objects[i].Shaders.PS = RRManager->GetResource<Rendering::RShader>(ps->ShaderHandle);

		Ret->Objects[i].Position = sceneObject->Position;
		Ret->Objects[i].Rotation = sceneObject->Rotation;
	}

	return Ret;
}

void Funky::Rendering::Renderer::DrawScene(IScene* InScene)
{
	DEBUG_SCOPE_TIMER(TEXT("Renderer::DrawScene"));

	Core::Memory::UniquePtr<RenderView> SceneToRender = CreateRenderScene(InScene);

	auto DefaultDS = RRManager->GetSwapchainDepthStencil();
	auto DefaultRT = RRManager->GetSwapchainRenderTarget();
	{
		GPU_MARKER("Begin Scene");
		RenderingBackend.BindRenderTarget(DefaultRT, DefaultDS);
		RenderingBackend.ClearRenderTarget(DefaultRT, { 0.392156899f, 0.584313750f, 0.929411829f });
		RenderingBackend.ClearDepthStencil(DefaultDS, 1.0f, 0u);
		RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
		//DirectX::XMMatrixLookAtLH()
	}
	
	{
		GPU_MARKER("MainPass");
		for (size i = 0; i < MAX_RENDER_PRIMITIVES; ++i)
		{
			RenderPrimitive * const CurrentObject = &SceneToRender->Objects[i];
	
			if(!CurrentObject->bIsValid) continue;
	
			CHECK(nullptr != CurrentObject->Mesh.VertexBuffer);
	
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

			//if(false)
			{
				DirectX::FXMVECTOR position_ws(PerObjectConstantBufferData.MVP.r[3]);
				DirectX::FXMVECTOR position_ss(DirectX::XMVector4Transform(position_ws, PerObjectConstantBufferData.MVP));

				DirectX::XMFLOAT4 position;
				DirectX::XMStoreFloat4(&position, position_ss);
			
				const auto x_ndc = position.x / position.w;
				const auto y_ndc = position.y / position.w;
				const float occlusion_treshold = 1.15f;

				if (x_ndc > occlusion_treshold || x_ndc < -occlusion_treshold)
					continue;
				if (y_ndc > occlusion_treshold || y_ndc < -occlusion_treshold)
					continue;
			}

			RenderingBackend.BindConstantBuffer(RenderingBackend::ShaderResourceStage::Vertex, PerObjectConstantBufferHandle, 0);
			RenderingBackend.UpdateConstantBuffer(PerObjectConstantBufferHandle, &PerObjectConstantBufferData);
	
			RenderingBackend.BindVertexShader(CurrentObject->Shaders.VS);
			RenderingBackend.BindPixelShader(CurrentObject->Shaders.PS);
	
			auto& Mesh = CurrentObject->Mesh;
			if (Mesh.IndexBuffer != nullptr)
				RenderingBackend.DrawIndexed(
					Mesh.VertexBuffer,
					Mesh.IndexBuffer
				);
			else
				RenderingBackend.Draw(Mesh.VertexBuffer);
	
		}
	}
	
	//
	//// Post Process
	////
	//{
	//	GPU_MARKER("Post Process");
	//
	//	//auto DefaultRT = RenderingBackend.GetResourceManager()->GetSwapchainRenderTarget();
	//	RenderingBackend.BindRenderTarget(DefaultRT);
	//
	//	RenderingBackend.BindTexture(RenderingBackend::ShaderResourceStage::Fragment, OffscreenRT, 0);
	//	RenderingBackend.BindTexture(RenderingBackend::ShaderResourceStage::Fragment, DefaultDS,   1);
	//
	//	RenderingBackend.BindVertexShader(PP->GetShaderLinkage().VS);
	//	RenderingBackend.BindPixelShader(PP->GetShaderLinkage().PS);
	//	RenderingBackend.Draw(PostProcess::GetPostProcessSurface());

	
	RenderingBackend.Present();
}
