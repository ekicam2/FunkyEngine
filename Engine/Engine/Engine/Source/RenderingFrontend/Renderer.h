#pragma once

#include "RenderingFrontend/IRenderer.h"

namespace Funky
{
	namespace Rendering
	{
		class Renderer : public IRenderer
		{
		public:
			FUNKY_DECLARE_RENDERER_CONSTRUCTOR(Renderer)

			virtual bool Init() override;
			virtual void Shutdown() override;

			virtual void DrawScene(class RenderScene* SceneToRender) override;
private:
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