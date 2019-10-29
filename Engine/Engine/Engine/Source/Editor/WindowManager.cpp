#include "WindowManager.h"

#include "FunkyEngine.h"
#include "Core/Containers.h"

//----------------------------------------------------------
// IWindow

Funky::Editor::IWindow::IWindow(str const& _WindowName)
	: WindowName(_WindowName)
	, Visible(false)
{
	FunkyEngine::GetEditorWindowManager()->RegisterWindow(this);
}

Funky::Editor::IWindow::~IWindow()
{
	FunkyEngine::GetEditorWindowManager()->UnregisterWindow(this);
}

//----------------------------------------------------------
// WindowManagerUI

Funky::Editor::WindowManagerUI::WindowManagerUI(WindowManager* _Parent)
{
	Parent = _Parent;
}

void Funky::Editor::WindowManagerUI::DrawGUI()
{
	ImGui::Begin("Window Manager");

	for (auto& Window : Parent->AllWindows)
	{
		bool EditVisible = Window->IsVisible();
		ImGui::Checkbox(Window->GetWindowName().c_str(), &EditVisible);

		if (EditVisible) Window->ShowGUI();
		else Window->HideGUI();
	}

	ImGui::End();
}

//----------------------------------------------------------
// WindowManager

Funky::Editor::WindowManager::WindowManager()
{
	UIWindow = new WindowManagerUI(this);
}

void Funky::Editor::WindowManager::DrawGUI()
{
	UIWindow->DrawGUI();

	for (auto& Window : AllWindows)
	{
		if(Window->IsVisible())
			Window->DrawGUI();
	}
}

void Funky::Editor::WindowManager::RegisterWindow(IWindow* Window)
{
	AllWindows.push_back(Window);
}

void Funky::Editor::WindowManager::UnregisterWindow(IWindow* Window)
{
	AllWindows.erase(
		std::remove(AllWindows.begin(), AllWindows.end(), Window)
	);
}


