#pragma once

#include "RenderingFrontend/IRenderer.h"
#include "RenderScene.h"

#include "Math/Matrix4D.h"

#define DEFINE_CONSTANT_BUFFER_BEGIN(BufferName) \
		struct BufferName {

//https://docs.microsoft.com/pl-pl/windows/desktop/direct3dhlsl/dx-graphics-hlsl-packing-rules			
#define DEFINE_CONSTANT_BUFFER_END(BufferName)	\
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



DEFINE_CONSTANT_BUFFER_BEGIN(BaseConstantBuffer)
	DEFINE_CONSTANT_BUFFER_MEMBER(Math::Mat4f, MVP, Math::Mat4f::Identity);
	DEFINE_CONSTANT_BUFFER_MEMBER(Math::Mat4f, Model, Math::Mat4f::Identity);
	DEFINE_CONSTANT_BUFFER_MEMBER(Math::Vec3f, LookAt, Math::Vec3f(0.0f, 0.0f, 0.0f));
	DEFINE_CONSTANT_BUFFER_PADDING1();
DEFINE_CONSTANT_BUFFER_END(BaseConstantBuffer)




namespace Funky
{
	namespace Rendering
	{
		class Renderer : public IRenderer
		{
			using ConstantBufferHandle = RBuffer*;

		public:
			FUNKY_DECLARE_RENDERER_CONSTRUCTOR(Renderer)

			virtual bool Init() override;
			virtual void Shutdown() override;


			virtual RenderScene* CreateRenderScene(IScene* Scene) override;
			virtual void DrawScene(RenderScene* SceneToRender) override;

		private:
			BaseConstantBuffer ConstantBufferData;
			ConstantBufferHandle ConstantBuffer;
		};
	}
}


/*
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

			BaseConstantBuffer MVPBuffer;
			Rendering::RenderingBackend::ConstantBuffer MVPBufferHandle = RenderingBackend::INVALID_INDEX;

			ShadowConstantBuffer ShadowCB;
			Rendering::RenderingBackend::ConstantBuffer ShadowCBHandle = RenderingBackend::INVALID_INDEX;

			Rendering::RenderingBackend::RenderTarget ShadowsRT = RenderingBackend::INVALID_INDEX;



*/