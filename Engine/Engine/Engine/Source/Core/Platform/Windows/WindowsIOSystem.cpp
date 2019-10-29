#include "WindowsIOSystem.h"

#include "Core/Platform/Platform.h"

void Funky::Core::IO::WindowsIOSystem::Update()
{
	constexpr u16 PressedMask = 1 << 15;

	Keys[(u32)IO::Key::A].IsPressed = (PressedMask & GetAsyncKeyState('A')) != 0;
	Keys[(u32)IO::Key::B].IsPressed = (PressedMask & GetAsyncKeyState('B')) != 0;
	Keys[(u32)IO::Key::C].IsPressed = (PressedMask & GetAsyncKeyState('C')) != 0;
	Keys[(u32)IO::Key::D].IsPressed = (PressedMask & GetAsyncKeyState('D')) != 0;
	Keys[(u32)IO::Key::E].IsPressed = (PressedMask & GetAsyncKeyState('E')) != 0;
	Keys[(u32)IO::Key::F].IsPressed = (PressedMask & GetAsyncKeyState('F')) != 0;
	Keys[(u32)IO::Key::G].IsPressed = (PressedMask & GetAsyncKeyState('G')) != 0;
	Keys[(u32)IO::Key::H].IsPressed = (PressedMask & GetAsyncKeyState('H')) != 0;
	Keys[(u32)IO::Key::I].IsPressed = (PressedMask & GetAsyncKeyState('I')) != 0;
	Keys[(u32)IO::Key::J].IsPressed = (PressedMask & GetAsyncKeyState('J')) != 0;
	Keys[(u32)IO::Key::K].IsPressed = (PressedMask & GetAsyncKeyState('K')) != 0;
	Keys[(u32)IO::Key::L].IsPressed = (PressedMask & GetAsyncKeyState('L')) != 0;
	Keys[(u32)IO::Key::M].IsPressed = (PressedMask & GetAsyncKeyState('M')) != 0;
	Keys[(u32)IO::Key::N].IsPressed = (PressedMask & GetAsyncKeyState('N')) != 0;
	Keys[(u32)IO::Key::O].IsPressed = (PressedMask & GetAsyncKeyState('O')) != 0;
	Keys[(u32)IO::Key::P].IsPressed = (PressedMask & GetAsyncKeyState('P')) != 0;
	Keys[(u32)IO::Key::Q].IsPressed = (PressedMask & GetAsyncKeyState('Q')) != 0;
	Keys[(u32)IO::Key::R].IsPressed = (PressedMask & GetAsyncKeyState('R')) != 0;
	Keys[(u32)IO::Key::S].IsPressed = (PressedMask & GetAsyncKeyState('S')) != 0;
	Keys[(u32)IO::Key::T].IsPressed = (PressedMask & GetAsyncKeyState('T')) != 0;
	Keys[(u32)IO::Key::U].IsPressed = (PressedMask & GetAsyncKeyState('U')) != 0;
	Keys[(u32)IO::Key::W].IsPressed = (PressedMask & GetAsyncKeyState('W')) != 0;
	Keys[(u32)IO::Key::X].IsPressed = (PressedMask & GetAsyncKeyState('X')) != 0;
	Keys[(u32)IO::Key::Y].IsPressed = (PressedMask & GetAsyncKeyState('Y')) != 0;
	Keys[(u32)IO::Key::Z].IsPressed = (PressedMask & GetAsyncKeyState('Z')) != 0;

	Keys[(u32)IO::Key::ARROW_UP].IsPressed		= (PressedMask & GetAsyncKeyState(VK_UP)) != 0;
	Keys[(u32)IO::Key::ARROW_DOWN].IsPressed	= (PressedMask & GetAsyncKeyState(VK_DOWN)) != 0;
	Keys[(u32)IO::Key::ARROW_LEFT].IsPressed	= (PressedMask & GetAsyncKeyState(VK_LEFT)) != 0;
	Keys[(u32)IO::Key::ARROW_RIGHT].IsPressed	= (PressedMask & GetAsyncKeyState(VK_RIGHT)) != 0;

	Keys[(u32)IO::Key::TAB].IsPressed	= (PressedMask & GetAsyncKeyState(VK_TAB)) != 0;
	Keys[(u32)IO::Key::SPACE].IsPressed = (PressedMask & GetAsyncKeyState(VK_SPACE)) != 0;
}
