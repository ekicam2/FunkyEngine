#include "EditorContext.h"

#include "FunkyEngine.h"

#include "Core/Platform/Platform.h"

#include "3rd/imgui/imgui.h"

//TODO(ekciam2): this whole file is fucking hacky to resolve that problem there should be much more layers

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
			MainCamera.Translate(Math::Vector3f(0.0f, 0.0f, 1.f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::S))
			MainCamera.Translate(Math::Vector3f(0.0f, 0.0f, -1.0f) *CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::A))
			MainCamera.Translate(Math::Vector3f(-1.0f, 0.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::D))
			MainCamera.Translate(Math::Vector3f(1.0f, 0.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::E))
			MainCamera.Translate(Math::Vector3f(0.0f, 1.0f, 0.0f) * CameraSpeed);
		if (IO.IsKeyPressed(Core::IO::Key::Q))
			MainCamera.Translate(Math::Vector3f(0.0f, -1.0f, 0.0f) * CameraSpeed);
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
				//Funky::Math::Vector3f(MouseOffset.y * CameraY, MouseOffset.x * CameraX, 0.0f),
				Funky::Math::Vector3f((float)MouseOffset.y, (float)MouseOffset.x, 0.0f),
				Funky::Math::Vector3f(1.0f, 0.0f, 0.0f),
				Funky::Math::Vector3f(89.0f, 0.0f, 0.0f)
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
