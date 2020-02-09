#pragma once

#include "Math/Camera.h"

#include "RenderingBackend/RenderingBackend.h"

#include "Core/Assets/ITexture.h"
#include "Core/Assets/Cubemap.h"

DEFINE_CONSTANT_BUFFER_BEGIN(BaseConstantBuffer)
BaseConstantBuffer() : LookAt(0.0f, 0.0f, 0.0f) {}
DirectX::XMMATRIX MVP;
DirectX::XMMATRIX Model;
Math::Vec3f LookAt;
DEFINE_CONSTANT_BUFFER_END(BaseConstantBuffer)

DEFINE_CONSTANT_BUFFER_BEGIN(ShadowConstantBuffer)
ShadowConstantBuffer() {}
DirectX::XMMATRIX View;
DirectX::XMMATRIX Projection;
DEFINE_CONSTANT_BUFFER_END(ShadowConstantBuffer)

namespace Funky
{
	namespace Rendering
	{
		class Renderer
		{
		public:
			Renderer(Rendering::RenderingBackend& RenderingBackend) : RenderingBackend(RenderingBackend) {}
			~Renderer() = default;

			void InitBuffers()
			{
				MVPBufferHandle = RenderingBackend.CreateConstantBuffer(sizeof(BaseConstantBuffer));
				ShadowCBHandle = RenderingBackend.CreateConstantBuffer(sizeof(ShadowConstantBuffer));
				ShadowsRT = RenderingBackend.CreateRenderTarget({ (u32)2048u, (u32)1024u });
			}

			// for now its ok but soon it will get complicated
			void DrawSceneFromView([[maybe_unused]]Math::Camera const* ViewCamera)
			{

			}

/*			{
				//			PREPARE FRAME
//				RenderingBackend.BindDefaultRenderTarget();
//				RenderingBackend.ClearRenderTargetWithColor({ 0.392156899f, 0.584313750f, 0.929411829f });
//				RenderingBackend.ClearDepthStencil(1.0f, 0u);
//				RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
//				//			PREPARE FRAME END 

				MVPBuffer.LookAt = ViewCamera->GetLookat().Normalized();

				for (u64 i = 0; i < RenderScene->SceneNodes.size(); ++i)
				{
					Scene::Drawable* Drawable = dynamic_cast<Scene::Drawable*>(RenderScene->SceneNodes[i]);
					if (Drawable && Drawable->bVisible)
					{
						if (!Drawable->Mesh.Data->HasValidProxy())
						{
							Drawable->Mesh.Data->Proxy = RenderingBackend.CreateMeshProxy(Drawable->Mesh.Data);
						}

						MVPBuffer.Model = DirectX::XMMatrixScaling(Drawable->Scale.X, Drawable->Scale.Y, Drawable->Scale.Z)
							* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Drawable->Rotation.X), Math::ToRad(Drawable->Rotation.Y), Math::ToRad(Drawable->Rotation.Z))
							* DirectX::XMMatrixTranslation(Drawable->Position.X, Drawable->Position.Y, Drawable->Position.Z);
						MVPBuffer.MVP = MVPBuffer.Model * ViewCamera->GetView() * ViewCamera->GetProjection();

						// DRAW SCENE
						RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));
						RenderingBackend.BindVertexShader(Drawable->Mesh.Mat->VS);
						RenderingBackend.BindPixelShader(Drawable->Mesh.Mat->PS);
						RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
						RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, ShadowCBHandle, 1u);
						RenderingBackend.BindTexture(Rendering::RenderingBackend::ShaderResourceStage::Fragment, ShadowsRT);
						RenderingBackend.DrawMesh(Drawable->Mesh.Data->Proxy);
						// DRAW SCENE END
					}
					else
					{
						//HACK(ekicam2): remove this later
						static Rendering::RenderingBackend::MeshProxy TerrainProxy = Rendering::RenderingBackend::INVALID_INDEX;

						Scene::Terrain* Terrain = dynamic_cast<Scene::Terrain*>(RenderScene->SceneNodes[i]);
						if (Terrain && Terrain->bVisible)
						{
							for (auto& TerrainNode : Terrain->TerrainNodes)
							{
								if (TerrainProxy == Rendering::RenderingBackend::INVALID_INDEX)
								{
									TerrainProxy = RenderingBackend.CreateMeshProxy(TerrainNode);
								}

								MVPBuffer.Model = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
								MVPBuffer.MVP = MVPBuffer.Model * ViewCamera->GetView() * ViewCamera->GetProjection();

								// DRAW SCENE
								RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

								RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
								RenderingBackend.DrawMesh(TerrainProxy);
								// DRAW SCENE END
							}
						}
					}
				}

				MVPBuffer.Model = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
				MVPBuffer.MVP = MVPBuffer.Model * ViewCamera->GetView() * ViewCamera->GetProjection();
				RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));

				if (RenderScene->SkySphere != nullptr)
				{
					static Rendering::RenderingBackend::MeshProxy SkySphereProxy = Rendering::RenderingBackend::INVALID_INDEX;

					if (SkySphereProxy == Rendering::RenderingBackend::INVALID_INDEX)
					{
						SkySphereProxy = RenderingBackend.CreateMeshProxy(RenderScene->SkySphere->Data);
					}

					RenderingBackend.BindVertexShader(RenderScene->SkySphere->Mat->VS);
					RenderingBackend.BindPixelShader(RenderScene->SkySphere->Mat->PS);
					RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
					RenderingBackend.BindTexture(Rendering::RenderingBackend::ShaderResourceStage::Fragment, ((Asset::CubemapTexture*)(RenderScene->SkySphere->Textures[0]))->Proxy);
					RenderingBackend.DrawMesh(SkySphereProxy);
				}

			}


			//void DrawShadowMap()
			//{
			//	//			PREPARE FRAME
			//	RenderingBackend.BindRenderTarget(ShadowsRT);
			//	RenderingBackend.ClearRenderTargetWithColor({ 0.0f, 0.0f, 0.0f }, ShadowsRT);
			//	RenderingBackend.ClearDepthStencil(1.0f, 0u);
			//	RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);
			//	//			PREPARE FRAME END 

			//	MVPBuffer.LookAt = ShadowCamera.GetLookat().Normalize();

			//	for (u64 i = 0; i < MainScene.SceneNodes.size(); ++i)
			//	{
			//		Scene::Drawable* Temp = dynamic_cast<Scene::Drawable*>(MainScene.SceneNodes[i]);
			//		if (Temp && Temp->bVisible)
			//		{
			//			if (!Temp->Mesh.Data->HasValidProxy())
			//			{
			//				Temp->Mesh.Data->Proxy = RenderingBackend.CreateMeshProxy(Temp->Mesh.Data);
			//			}

			//			MVPBuffer.Model = DirectX::XMMatrixScaling(Temp->Scale.X, Temp->Scale.Y, Temp->Scale.Z)
			//				* DirectX::XMMatrixRotationRollPitchYaw(Math::ToRad(Temp->Rotation.X), Math::ToRad(Temp->Rotation.Y), Math::ToRad(Temp->Rotation.Z))
			//				* DirectX::XMMatrixTranslation(Temp->Position.X, Temp->Position.Y, Temp->Position.Z);
			//			MVPBuffer.MVP = MVPBuffer.Model * ShadowCamera.GetView() * ShadowCamera.GetProjection();

			//			// DRAW SCENE
			//			RenderingBackend.UpdateConstantBuffer(MVPBufferHandle, (Rendering::RenderingBackend::ConstantBufferData)(&MVPBuffer));


			//			RenderingBackend.BindVertexShader(DepthMaterial.VS);
			//			RenderingBackend.BindPixelShader(DepthMaterial.PS);
			//			RenderingBackend.BindConstantBuffer(Rendering::RenderingBackend::ShaderResourceStage::Vertex, MVPBufferHandle);
			//			RenderingBackend.DrawMesh(Temp->Mesh.Data->Proxy);
			//			// DRAW SCENE END
			//		}
			//}
			*/
private:
			Rendering::RenderingBackend& RenderingBackend;

			BaseConstantBuffer MVPBuffer;
			Rendering::RenderingBackend::ConstantBuffer MVPBufferHandle = RenderingBackend::INVALID_INDEX;

			ShadowConstantBuffer ShadowCB;
			Rendering::RenderingBackend::ConstantBuffer ShadowCBHandle = RenderingBackend::INVALID_INDEX;

			Rendering::RenderingBackend::RenderTarget ShadowsRT = RenderingBackend::INVALID_INDEX;
		};
	}
}