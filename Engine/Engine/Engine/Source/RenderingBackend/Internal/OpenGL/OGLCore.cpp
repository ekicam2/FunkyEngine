#include "RenderingBackend/Internal/OpenGL/OGLCore.h"

#include "LogMacros.h"
#include "DebugMacros.h"

#include "Templates.h"

#define FUNKY_GLSL_MIN_VERSION "#version 130"

// Currently linking statically to the 	Opengl32.lib.
// It would be cool if in the nearest future it will be changed to Opengl32.dll.
// So we will be able to attach and detach whole OGL module runtime.

namespace Funky
{
	namespace Rendering
	{
		bool OGL::Init(HWND hwnd)
		{
			WindowHandle = hwnd;

			if (!gl3wInit())
			{
				LOG_ERROR(TEXT("Couldn't load OpenGL core"));
				return false;
			}

			if (!gl3wIsSupported(4, 1))
			{
				LOG_ERROR(TEXT("OpenGL 4.1 is not supported"));
				return false;
			}

			if (CreateDeviceAndSwapchain(hwnd))
			{
				return InitSwapchain();
			}

			return false;
		}

		RenderingBackend::API OGL::GetBackendAPI() const
		{
			return RenderingBackend::API::OGL;
		}

		void OGL::OnViewportResized([[maybe_unused]]Math::Vec2u const & NewSize)
		{
			ASSERT(false, TEXT("Not supported at the moment."));

			//InitSwapchain();
			//
			//ImGui_ImplOpenGL3_Shutdown();
			//ImGui_ImplOpenGL3_Init(FUNKY_GLSL_MIN_VERSION);
		}

		RenderingBackend::RenderTarget OGL::CreateRenderTarget([[maybe_unused]] Math::Vec2u const & Size /* TODO(ekicam2): I woild like to specify format*/)
		{
			//TODO(ekicam2): implement
			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::VertexShader OGL::CreateVertexShader([[maybe_unused]] byte* VertexShaderData, [[maybe_unused]] u64 DataSize)
		{
			//TODO(ekicam2): implement
			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::PixelShader OGL::CreatePixelShader([[maybe_unused]] byte* PixelShaderData, [[maybe_unused]] u64 DataSize)
		{
			//TODO(ekicam2): implement
			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::ConstantBuffer OGL::CreateConstantBuffer([[maybe_unused]] size SizeOfConstantBuffer)
		{
			//TODO(ekicam2): another layer of indirection is needed, ConstantBuffer in OGL is associated with program itself => need to cache it on the side
			return RenderingBackend::INVALID_INDEX;
		}

		RenderingBackend::Texture OGL::CreateTexture2D(byte const * const Data, Math::Vec2u const & Size)
		{
			Textures.push_back(Move(OGLGPUTexture()));

			const u64 Index = Textures.size() - 1;

			glGenTextures(GL_TEXTURE_2D, &Textures[Index].Texture);
			glBindTexture(GL_TEXTURE_2D, Textures[Index]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Size.X, Size.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);

			return Index;
		}

		RenderingBackend::Texture OGL::CreateCubemap(byte const * const Data, Math::Vec2u const & Size)
		{
			Textures.push_back(Move(OGLGPUTexture()));

			const u64 Index = Textures.size() - 1;

			glGenTextures(GL_TEXTURE_CUBE_MAP, &Textures[Index].Texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, Textures[Index]);

			for (u8 i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Size.X, Size.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, Data + (Size.X * Size.Y * i));
			}

			return Index;
		}


		void OGL::BindRenderTarget([[maybe_unused]] RenderingBackend::RenderTarget RenderTargetToBind)
		{

		}

		void OGL::BindDefaultRenderTarget()
		{
			
		}

		void OGL::ClearRenderTargetWithColor(Math::Vec3f const & Color, RenderingBackend::RenderTarget RenderTargetToClear)
		{
			//ID3D11RenderTargetView* FinalTarget =  ? pRenderTarget.Get() : RenderTargets[RenderTargetToClear].pRenderTargetView.Get();

			if (RenderTargetToClear.IsValid())
			{
				//currently we dont need to do anything as long we are using only one target
			}
			else
			{
				ASSERT(false, TEXT("not supported atm"));
				//RenderTargets[RenderTargetToClear];
			}


			glClearColor(Color.X, Color.Y, Color.Z, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void OGL::ClearDepthStencil(float Depth, float Stencil)
		{
			glClearDepth(Depth);
			glClearStencil((GLuint)Stencil);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void OGL::SetPrimitiveTopology([[maybe_unused]] RenderingBackend::PrimitiveTopology NewTopology)
		{
			ASSERT(false, TEXT("not supported atm"));
		}

		void OGL::UpdateConstantBuffer([[maybe_unused]] RenderingBackend::ConstantBuffer ConstantBuffer, [[maybe_unused]] RenderingBackend::ConstantBufferData Data)
		{
			//TODO(ekicam2): another layer of indirection is needed, ConstantBuffer in OGL is associated with program itself => need to cache it on the side
		}

		void OGL::BindConstantBuffer([[maybe_unused]] RenderingBackend::ShaderResourceStage Stage, [[maybe_unused]] Rendering::RenderingBackend::ConstantBuffer const & Buffers, [[maybe_unused]] u32 StartIndex)
		{
			//TODO(ekicam2): another layer of indirection is needed, ConstantBuffer in OGL is associated with program itself => need to cache it on the side
		}

		void OGL::BindVertexShader([[maybe_unused]] RenderingBackend::VertexShader VertexShaderToBind)
		{
			//TODO(ekicam2): store it as well as pixel shader and just before drawing find or create program for these two
		}

		void OGL::BindPixelShader([[maybe_unused]] RenderingBackend::PixelShader PixelShaderToBind)
		{
			//TODO(ekicam2): store it as well as pixel shader and just before drawing find or create program for these two
		}

		void OGL::Present()
		{
			HDC ourWindowHandleToDeviceContext = GetDC(WindowHandle);
			SwapBuffers(ourWindowHandleToDeviceContext);
		}


		bool OGL::CreateDeviceAndSwapchain(HWND hWnd)
		{
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

			int  letWindowsChooseThisPixelFormat;
			letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

			HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);

			wglDeleteContext(ourOpenGLRenderingContext);
			PostQuitMessage(0);

			return true;
		}

		bool OGL::InitSwapchain()
		{
			// no such a feature as swapchain in OGL
			return true;
		}

		void OGL::FreeSwapchain()
		{
			// no such a feature as swapchain in OGL
		}

		void OGL::BindTexture([[maybe_unused]] RenderingBackend::ShaderResourceStage Stage, RenderingBackend::Texture const & Texture, [[maybe_unused]] u32 StartIndex /*= 0u*/)
		{
			glBindTexture(GL_TEXTURE_2D, Textures[Texture]);
		}

		void OGL::BindTexture([[maybe_unused]] RenderingBackend::ShaderResourceStage Stage, RenderingBackend::RenderTarget const & Texture, [[maybe_unused]] u32 StartIndex /*= 0u*/)
		{
			glBindTexture(GL_TEXTURE_2D, RenderTargets[Texture]);
		}
	}
}