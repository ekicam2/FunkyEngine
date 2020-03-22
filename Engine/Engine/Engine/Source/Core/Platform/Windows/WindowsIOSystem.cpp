#include "WindowsIOSystem.h"

#include "Core/Platform/Platform.h"

void Funky::Core::IO::WindowsIOSystem::Update()
{
	Frame();

	constexpr u16 PressedMask = 1 << 15;

	Keys[(u32)IO::EKey::A].IsPressed = (PressedMask & GetAsyncKeyState('A')) != 0;
	Keys[(u32)IO::EKey::B].IsPressed = (PressedMask & GetAsyncKeyState('B')) != 0;
	Keys[(u32)IO::EKey::C].IsPressed = (PressedMask & GetAsyncKeyState('C')) != 0;
	Keys[(u32)IO::EKey::D].IsPressed = (PressedMask & GetAsyncKeyState('D')) != 0;
	Keys[(u32)IO::EKey::E].IsPressed = (PressedMask & GetAsyncKeyState('E')) != 0;
	Keys[(u32)IO::EKey::F].IsPressed = (PressedMask & GetAsyncKeyState('F')) != 0;
	Keys[(u32)IO::EKey::G].IsPressed = (PressedMask & GetAsyncKeyState('G')) != 0;
	Keys[(u32)IO::EKey::H].IsPressed = (PressedMask & GetAsyncKeyState('H')) != 0;
	Keys[(u32)IO::EKey::I].IsPressed = (PressedMask & GetAsyncKeyState('I')) != 0;
	Keys[(u32)IO::EKey::J].IsPressed = (PressedMask & GetAsyncKeyState('J')) != 0;
	Keys[(u32)IO::EKey::K].IsPressed = (PressedMask & GetAsyncKeyState('K')) != 0;
	Keys[(u32)IO::EKey::L].IsPressed = (PressedMask & GetAsyncKeyState('L')) != 0;
	Keys[(u32)IO::EKey::M].IsPressed = (PressedMask & GetAsyncKeyState('M')) != 0;
	Keys[(u32)IO::EKey::N].IsPressed = (PressedMask & GetAsyncKeyState('N')) != 0;
	Keys[(u32)IO::EKey::O].IsPressed = (PressedMask & GetAsyncKeyState('O')) != 0;
	Keys[(u32)IO::EKey::P].IsPressed = (PressedMask & GetAsyncKeyState('P')) != 0;
	Keys[(u32)IO::EKey::Q].IsPressed = (PressedMask & GetAsyncKeyState('Q')) != 0;
	Keys[(u32)IO::EKey::R].IsPressed = (PressedMask & GetAsyncKeyState('R')) != 0;
	Keys[(u32)IO::EKey::S].IsPressed = (PressedMask & GetAsyncKeyState('S')) != 0;
	Keys[(u32)IO::EKey::T].IsPressed = (PressedMask & GetAsyncKeyState('T')) != 0;
	Keys[(u32)IO::EKey::U].IsPressed = (PressedMask & GetAsyncKeyState('U')) != 0;
	Keys[(u32)IO::EKey::W].IsPressed = (PressedMask & GetAsyncKeyState('W')) != 0;
	Keys[(u32)IO::EKey::X].IsPressed = (PressedMask & GetAsyncKeyState('X')) != 0;
	Keys[(u32)IO::EKey::Y].IsPressed = (PressedMask & GetAsyncKeyState('Y')) != 0;
	Keys[(u32)IO::EKey::Z].IsPressed = (PressedMask & GetAsyncKeyState('Z')) != 0;

	Keys[(u32)IO::EKey::ARROW_UP].IsPressed		= (PressedMask & GetAsyncKeyState(VK_UP)) != 0;
	Keys[(u32)IO::EKey::ARROW_DOWN].IsPressed	= (PressedMask & GetAsyncKeyState(VK_DOWN)) != 0;
	Keys[(u32)IO::EKey::ARROW_LEFT].IsPressed	= (PressedMask & GetAsyncKeyState(VK_LEFT)) != 0;
	Keys[(u32)IO::EKey::ARROW_RIGHT].IsPressed	= (PressedMask & GetAsyncKeyState(VK_RIGHT)) != 0;

	Keys[(u32)IO::EKey::SHIFT].IsPressed = (PressedMask & GetAsyncKeyState(VK_SHIFT)) != 0;
	Keys[(u32)IO::EKey::TAB].IsPressed = (PressedMask & GetAsyncKeyState(VK_TAB)) != 0;
	Keys[(u32)IO::EKey::SPACE].IsPressed = (PressedMask & GetAsyncKeyState(VK_SPACE)) != 0;
}
