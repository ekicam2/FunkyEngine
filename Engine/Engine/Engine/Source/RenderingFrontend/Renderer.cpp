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
	//DEBUG_SCOPE_TIMER(TEXT("Renderer::CreateRenderScene"));

	darray<Funky::VisibleObject> sceneObjects = Move(InScene->GetVisibleObjects());

	RenderView* ret = new RenderView();
	ret->Camera = InScene->GetCamera();

	ret->View			= ret->Camera->GetView();
	ret->Projection		= ret->Camera->GetProjection();
	ret->ViewProjection = ret->View * ret->Projection;

	const size sceneObjectsNum = sceneObjects.size();
	for (size i = 0; (i < sceneObjectsNum && i < MAX_RENDER_PRIMITIVES); ++i)
	{
		VisibleObject * const sceneObject = &sceneObjects[i];
		{
			auto mesh		= PrepareMeshFor(sceneObject);
			auto [vs, ps]	= PrepareMaterialFor(sceneObject);

			ret->Objects[i].bIsValid = true;

			ret->Objects[i].Mesh.VertexBuffer	= RRManager->GetResource<Rendering::RBuffer>(mesh->VertexBuffer);
			ret->Objects[i].Mesh.IndexBuffer	= mesh->GetIndicesCount() > 0u ? RRManager->GetResource<Rendering::RBuffer>(mesh->IndexBuffer) : nullptr;

			ret->Objects[i].Shaders.VS = RRManager->GetResource<Rendering::RShader>(vs->ShaderHandle);
			ret->Objects[i].Shaders.PS = RRManager->GetResource<Rendering::RShader>(ps->ShaderHandle);

			ret->Objects[i].Position = sceneObject->Position;
			ret->Objects[i].Rotation = sceneObject->Rotation;
			ret->Objects[i].Scale = sceneObject->Scale;
		}

		auto ParentModel = ret->Objects[i].GetModelMatrixNoScale();
		if (size childrenCount = sceneObject->ChildrenCount; childrenCount > 0)
		{
			size j = 0;
			while (i < sceneObjectsNum && i < MAX_RENDER_PRIMITIVES && j < (i + childrenCount))
			{
				++j;

				VisibleObject* const currentVisible = &sceneObjects[i + j];
				auto mesh							= PrepareMeshFor(currentVisible);
				auto [vs, ps]						= PrepareMaterialFor(currentVisible);

				ret->Objects[i + j].bIsValid	= true;
				ret->Objects[i + j].ParentModel = ParentModel;

				ret->Objects[i + j].Mesh.VertexBuffer = RRManager->GetResource<Rendering::RBuffer>(mesh->VertexBuffer);
				ret->Objects[i + j].Mesh.IndexBuffer  = mesh->GetIndicesCount() > 0u ? RRManager->GetResource<Rendering::RBuffer>(mesh->IndexBuffer) : nullptr;

				ret->Objects[i + j].Shaders.VS = RRManager->GetResource<Rendering::RShader>(vs->ShaderHandle);
				ret->Objects[i + j].Shaders.PS = RRManager->GetResource<Rendering::RShader>(ps->ShaderHandle);

				ret->Objects[i + j].Position = currentVisible->Position;
				ret->Objects[i + j].Rotation = currentVisible->Rotation;
				ret->Objects[i + j].Scale	 = currentVisible->Scale;
			};

			i += j;
		}

		
	}

	return ret;
}

void Funky::Rendering::Renderer::DrawScene(IScene* InScene)
{
	//DEBUG_SCOPE_TIMER(TEXT("Renderer::DrawScene"));

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
		DirectX::XMMATRIX vp;
		{
			GPU_MARKER("Begin main pass view");
	
			vp = SceneToRender->Camera->GetView() * SceneToRender->Camera->GetProjection();
	
			PerViewConstantBufferData.VP = vp;
			PerViewConstantBufferData.CameraPosition = Math::Vec4f(SceneToRender->Camera->GetPosition(), 1.0f);
			PerViewConstantBufferData.CameraForward = Math::Vec4f(SceneToRender->Camera->GetForward(), 0.0f);
			RenderingBackend.BindConstantBuffer(RenderingBackend::ShaderResourceStage::Fragment, PerViewConstantBufferHandle, 0);
			RenderingBackend.UpdateConstantBuffer(PerViewConstantBufferHandle, &PerViewConstantBufferData);
		}
	
		for (size i = 0; i < MAX_RENDER_PRIMITIVES; ++i)
		{
			RenderPrimitive * const CurrentObject = &SceneToRender->Objects[i];
	
			if(!CurrentObject->bIsValid) break;
	
			CHECK(nullptr != CurrentObject->Mesh.VertexBuffer);
	
			DirectX::XMMATRIX model = DirectX::XMMatrixIdentity();
			//DirectX::XMMATRIX model = CurrentObject->ParentModel;

			/*Math::Vec3f ParentPos(
				DirectX::XMVectorGetX(CurrentObject->ParentModel.r[3])
				, DirectX::XMVectorGetY(CurrentObject->ParentModel.r[3])
				, DirectX::XMVectorGetZ(CurrentObject->ParentModel.r[3])
			);*/
			
			model *= DirectX::XMMatrixRotationRollPitchYaw(
				Math::ToRad(CurrentObject->Rotation.X), 
				Math::ToRad(CurrentObject->Rotation.Y), 
				Math::ToRad(CurrentObject->Rotation.Z)
			);
			model *= DirectX::XMMatrixTranslation(
				/*ParentPos.X +*/ CurrentObject->Position.X, 
				/*ParentPos.Y +*/ CurrentObject->Position.Y, 
				/*ParentPos.Z +*/ CurrentObject->Position.Z
			);
			model *= DirectX::XMMatrixScaling(CurrentObject->Scale, CurrentObject->Scale, CurrentObject->Scale); 

			model *= CurrentObject->ParentModel;

			PerObjectConstantBufferData.MVP	  = model * vp;
			PerObjectConstantBufferData.Model = model;
			
#ifdef POOR_CULLING
			{
				DirectX::FXMVECTOR position_ws(PerObjectConstantBufferData.MVP.r[3]);
				DirectX::FXMVECTOR position_ss(DirectX::XMVector4Transform(position_ws, PerObjectConstantBufferData.MVP));
	
				DirectX::XMFLOAT4 position;
				DirectX::XMStoreFloat4(&position, position_ss);
			
				const auto x_ndc = position.x / position.w;
				const auto y_ndc = position.y / position.w;
				[[maybe_unused]]const auto z_ndc = position.z / position.w;
				const float occlusion_treshold = 1.15f;
	
				if (x_ndc > occlusion_treshold || x_ndc < -occlusion_treshold)
					continue;
				if (y_ndc > occlusion_treshold || y_ndc < -occlusion_treshold)
					continue;
				if (z_ndc > 1.0f)
					continue;
			}
#endif
	
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

Funky::Asset::StaticMesh* Funky::Rendering::Renderer::PrepareMeshFor(VisibleObject* visibleObj)
{
	auto& ar = AssetRegistry::GetInstance();
	Asset::StaticMesh* mesh = ar.GetAsset<Asset::StaticMesh>(visibleObj->Mesh);
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

Funky::Rendering::Renderer::ShadersBundle Funky::Rendering::Renderer::PrepareMaterialFor(VisibleObject* visibleObj)
{
	auto& ar = AssetRegistry::GetInstance();
	auto material = ar.GetAsset<Asset::Material>(visibleObj->Material);
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

	return ShadersBundle{ vs, ps };
};
