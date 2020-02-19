#pragma once

#include "Core/Memory/UniquePtr.h"
#include "Core/Containers.h"

#include "UserFramework/Scene.h"

namespace Funky
{
	class SceneManager
	{
	public:
		SceneManager();
		Scene * GetCurrentScene() const { return Scenes[0].Get(); }

		void Tick(f32 Delta);
	private:
		darray<Core::Memory::UniquePtr<Scene>> Scenes;
	};
}