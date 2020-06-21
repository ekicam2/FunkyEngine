#pragma once

#include "RenderingBackend/RenderingBackend.h"

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

namespace Funky
{
	struct IScene;

	namespace Rendering
	{
		struct RenderView;

		class IRenderer
		{
		public:
			IRenderer(Rendering::RenderingBackend& RenderingBackend) : RenderingBackend(RenderingBackend) {}
			virtual ~IRenderer() = default;

			virtual bool Init(Rendering::RenderingBackend::RenderingBackendInitResult *renderingBackendInitResult) = 0;
			virtual void Shutdown() = 0;

			virtual RenderView* CreateRenderScene(IScene* Scene) = 0;
			virtual void DrawScene(IScene* InScene) = 0;
		protected:
			Rendering::RenderingBackend& RenderingBackend;
		};

	}
}

#define FUNKY_DEFINE_RENDERER_CONSTRUCTOR(__name__) \
__name__(Rendering::RenderingBackend& RenderingBackend) : IRenderer(RenderingBackend) {}