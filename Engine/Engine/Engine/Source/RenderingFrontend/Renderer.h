#pragma once

#include "RenderingFrontend/IRenderer.h"
#include "RenderView.h"

#include "Math/Matrix4.h"

#include <DirectXMath.h>


#include "Core/Assets/Material.h"
#include "RenderingFrontend/RenderingResourceManager.h"


#define DEFINE_CONSTANT_BUFFER_STRUCT_BEGIN(BufferName) \
		struct BufferName {

//https://docs.microsoft.com/pl-pl/windows/desktop/direct3dhlsl/dx-graphics-hlsl-packing-rules			
#define DEFINE_CONSTANT_BUFFER_STRUCT_END(BufferName)	\
		};										\
		static_assert((sizeof(BufferName) % 16) == 0, "Constant Buffer size must be 16-byte aligned");	

#define DEFINE_CONSTANT_BUFFER_MEMBER(Type, Name, DefaultValue) \
	Type Name = DefaultValue

#define DEFINE_CONSTANT_BUFFER_PADDING1() \
	DEFINE_CONSTANT_BUFFER_MEMBER(f32, __padding, 0.1234567f);

#define DEFINE_CONSTANT_BUFFER_PADDING2()	\
	DEFINE_CONSTANT_BUFFER_PADDING1()		\
	DEFINE_CONSTANT_BUFFER_PADDING1()		


#define DEFINE_CONSTANT_BUFFER_PADDING3()	\
	DEFINE_CONSTANT_BUFFER_PADDING2()		\
	DEFINE_CONSTANT_BUFFER_PADDING1()


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

			Core::Memory::UniquePtr<RenderingResourcesManager> RRManager;

			Resource::ID OffscreenRT = Rendering::Resource::ID::Zero;
			Core::Memory::UniquePtr<PostProcess> PP;
		};
	}
}