#ifdef FUNKY_EDITOR

#include "EditorContext.h"

#include "FunkyEngine.h"

#include "Core/Platform/Platform.h"

#include "Utils/MeshUtils.h"

#include "3rd/imgui/imgui.h"

//TODO(ekciam2): at the moment the only problem I can see is that direct winapi call in the Update method

Funky::Editor::EditorContext::EditorContext() 
	: IWindow("Editor")
	, MainCamera()
{ 
	ShowGUI(); 

	RECT ClientArea;
	GetClientRect(FunkyEngine::GetEngine()->hWnd, &ClientArea);
	f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
	f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);

	MainCamera.MakePerepective(DeltaX / DeltaY, 90.0f, 0.01f, 3000.0f);
}

bool Funky::Editor::EditorContext::Init()
{
		//RECT ClientArea;
		//GetClientRect(hWnd, &ClientArea);
		//f32 DeltaX = (f32)(ClientArea.right - ClientArea.left);
		//f32 DeltaY = (f32)(ClientArea.bottom - ClientArea.top);


		EditorCube = std::make_unique<Asset::RawMesh>(MeshUtils::CreateCube());
		EditorSphere = std::make_unique<Asset::RawMesh>(MeshUtils::CreateSphere(2000.0f, true));

		str Textures[6] = {
 			"RealData/Textures/mp_troubled/troubled-waters_ft.tga",
			"RealData/Textures/mp_troubled/troubled-waters_bk.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_up.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_dn.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_rt.tga",
 			"RealData/Textures/mp_troubled/troubled-waters_lf.tga"
 		};
		Asset::CubemapTexture* SkyTexture = Asset::CubemapTexture::CreateFromFile(Textures);
		SkyTexture->Proxy = FunkyEngine::GetEngine()->GetRenderingBackend()->CreateCubemap(SkyTexture->GetData(), SkyTexture->Size);

		
		Asset::Material* SkyMaterial	= FunkyEngine::GetEngine()->GetAssetRegistry()->GetByName<Asset::Material>("Sky");
		Asset::Material* LitMaterial	= FunkyEngine::GetEngine()->GetAssetRegistry()->GetByName<Asset::Material>("Sample");


		// Math::Camera ShadowCamera(DeltaX / DeltaY, 90.0f, 1.0f, 26.5f);
		// ShadowCamera.MakeOrtho(DeltaX / DeltaY, 1.0f, 1.0, 15.0f);
		// ShadowCamera.Translate({ 10.0f, 0.0f, 0.0f });
		// ShadowCamera.Rotate({ 0.0f, -90.0f, 0.0f });

		//ShadowCB.Projection = ShadowCamera.GetProjection();
		//ShadowCB.View = ShadowCamera.GetView();
		//RenderingBackend.UpdateConstantBuffer(ShadowCBHandle, (Rendering::RenderingBackend::ConstantBufferData)(&ShadowCB));

		for (u8 i = 0; i < 4; ++i)
		{
			auto NewDrawable = new Scene::Drawable();
			NewDrawable->Mesh.Mat = LitMaterial;
			//NewDrawable->Mesh.Targets.push_back(ShadowsRT);
			NewDrawable->Mesh.Data = EditorCube.get();
			NewDrawable->Name = str("drawableObj_").append(std::to_string(i));
			NewDrawable->Position = Math::Vec3f(0.0f, 0.0f, 1.0f) * i;
			FunkyEngine::GetEngine()->GetCurrentScene()->SceneNodes.push_back(NewDrawable);
		}
		
		FunkyEngine::GetEngine()->GetCurrentScene()->SkySphere = new Scene::MeshComponent();
		FunkyEngine::GetEngine()->GetCurrentScene()->SkySphere->Data = EditorSphere.get();
		FunkyEngine::GetEngine()->GetCurrentScene()->SkySphere->Mat = SkyMaterial;
		FunkyEngine::GetEngine()->GetCurrentScene()->SkySphere->Textures.push_back((ITexture*)SkyTexture);

		return true;
}

void Funky::Editor::EditorContext::DrawGUI()
{
	ImGuiWindowFlags WindowFlags = 0;
	WindowFlags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(GetWindowName().c_str(), NULL, WindowFlags);
	ImGui::DragFloat("Camera speed", &CameraSpeed, 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Camera X speed", &CameraX, 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Camera Y speed", &CameraY, 0.01f, 0.0f, 10.10f, "%.2f");
	ImGui::End();
}

void Funky::Editor::EditorContext::Update()
{
	constexpr byte PressedMask = 1 << 7;

	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		auto& IO = *FunkyEngine::GetIO(); 

		if (IO.IsKeyPressed(Core::IO::Key::W))
			MainCamera.Translate(Math::Vec3f(0.0f, 0.0f, 1.f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::S))
			MainCamera.Translate(Math::Vec3f(0.0f, 0.0f, -1.0f) *CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::A))
			MainCamera.Translate(Math::Vec3f(-1.0f, 0.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::D))
			MainCamera.Translate(Math::Vec3f(1.0f, 0.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::E))
			MainCamera.Translate(Math::Vec3f(0.0f, 1.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::Q))
			MainCamera.Translate(Math::Vec3f(0.0f, -1.0f, 0.0f) * CameraSpeed);
	}

	if (!ImGui::GetIO().WantCaptureMouse && PressedMask & GetKeyState(VK_RBUTTON))
	{
		RECT WindowRect = { 0 };
		GetWindowRect(FunkyEngine::GetEngine()->hWnd, &WindowRect); //get window rect of control relative to screen
		POINT CursorPosition = { 0, 0 };
		GetCursorPos(&CursorPosition);
		POINT WindowCenter = {
			WindowRect.left + ((WindowRect.right - WindowRect.left) / 2),
			WindowRect.top + ((WindowRect.bottom - WindowRect.top) / 2)
		};

		POINT MouseOffset = {
			CursorPosition.x - WindowCenter.x,
			CursorPosition.y - WindowCenter.y
		};

		if (bPrevFrameRMB)
		{
			MainCamera.RotateClamped(
				//Funky::Math::Vec3f(MouseOffset.y * CameraY, MouseOffset.x * CameraX, 0.0f),
				Math::Vec3f((float)MouseOffset.y, (float)MouseOffset.x, 0.0f),
				Math::Vec3f(1.0f, 0.0f, 0.0f),
				Math::Vec3f(89.0f, 0.0f, 0.0f)
			);
		}

		SetCursorPos(WindowCenter.x, WindowCenter.y);

		bPrevFrameRMB = true;
	}
	else
	{
		bPrevFrameRMB = false;
	}
}
#endif // FUNKY_EDITOR
