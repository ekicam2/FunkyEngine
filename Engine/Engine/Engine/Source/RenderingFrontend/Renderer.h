#pragma once

#include "RenderingFrontend/IRenderer.h"
#include "RenderView.h"

#include "Math/Matrix4.h"

#include <DirectXMath.h>


#include "Core/Assets/Material.h"
#include "RenderingFrontend/RenderingResourceManager.h"

DEFINE_CONSTANT_BUFFER_STRUCT_BEGIN(PerViewConstantBuffer)
	DEFINE_CONSTANT_BUFFER_MEMBER(Math::Vec4f, CameraPosition, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	DEFINE_CONSTANT_BUFFER_MEMBER(Math::Vec4f, CameraForward, Math::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	DEFINE_CONSTANT_BUFFER_MEMBER(DirectX::XMMATRIX, VP, DirectX::XMMatrixIdentity());
DEFINE_CONSTANT_BUFFER_STRUCT_END(PerViewConstantBuffer)


DEFINE_CONSTANT_BUFFER_STRUCT_BEGIN(PerObjectConstantBuffer)
	DEFINE_CONSTANT_BUFFER_MEMBER(DirectX::XMMATRIX, MVP, DirectX::XMMatrixIdentity());
	DEFINE_CONSTANT_BUFFER_MEMBER(DirectX::XMMATRIX, Model, DirectX::XMMatrixIdentity());
DEFINE_CONSTANT_BUFFER_STRUCT_END(PerObjectConstantBuffer)

#define DEFINE_CONSTANT_BUFFER(Name)  \
	Name Name##Data;				  \
	ConstantBufferHandle Name##Handle;


namespace Funky
{
	namespace Rendering
	{
		class Renderer : public IRenderer
		{
			using ConstantBufferHandle = RBuffer*;

		public:
			FUNKY_DEFINE_RENDERER_CONSTRUCTOR(Renderer)

			virtual bool Init(Rendering::RenderingBackend::RenderingBackendInitResult* renderingBackendInitResult) override;
			virtual void Shutdown() override;


			virtual RenderView* CreateRenderScene(IScene* Scene) override;
			virtual void DrawScene(IScene* InScene) override;

		private:
			DEFINE_CONSTANT_BUFFER(PerViewConstantBuffer);
			DEFINE_CONSTANT_BUFFER(PerObjectConstantBuffer);

			Asset::StaticMesh* PrepareMeshFor(VisibleObject* visibleObj);
			struct ShadersBundle
			{
				Asset::Shader* VS, * PS;
			};
			ShadersBundle PrepareMaterialFor(VisibleObject* visibleObj);

			Core::Memory::UniquePtr<RenderingResourcesManager> RRManager;

			Resource::ID OffscreenRT = Rendering::Resource::ID::Zero;
			Core::Memory::UniquePtr<PostProcess> PP;
		};
	}
}