#include "IIOSystem.h"



void Funky::Core::IO::IIOSystem::Frame()
{
	for (auto& KeyState : Keys)
	{
		KeyState.WasPressedLastFrame = KeyState.IsPressed;
	}
}
