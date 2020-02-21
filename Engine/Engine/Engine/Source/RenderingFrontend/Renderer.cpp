#include "Renderer.h"
#include "RenderingResourceManager.h"

bool Funky::Rendering::Renderer::Init()
{
	return true;
}

void Funky::Rendering::Renderer::Shutdown()
{

}

void Funky::Rendering::Renderer::DrawScene(class RenderScene* SceneToRender)
{
	(void*)SceneToRender;

	auto RT = RenderingBackend.GetResourceManager()->GetResourceAt<RRenderTarget>(0);
	RenderingBackend.ClearRenderTarget(RT, { 0.392156899f, 0.584313750f, 0.929411829f });
	//RenderingBackend.ClearDepthStencil(1.0f, 0u);
	RenderingBackend.SetPrimitiveTopology(Rendering::RenderingBackend::PrimitiveTopology::Trianglelist);


	RenderingBackend.Present();
}
