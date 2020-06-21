#pragma once

#include "Core/Memory/UniquePtr.h"
#include "Core/Containers.h"

#include "UserFramework/Scene.h"

namespace Funky
{
	class SceneManager
	{
	public:
		struct SceneHandle 
		{
			SceneHandle(u64 newValue) :Value(newValue) { }
			u64 Value = SceneHandle::Zero;
			inline static u64 Zero = (u64)-1;

			operator u64() const { return Value; }
		};

		IScene* GetCurrentScene() const { return Scenes[CurrentScene].Get(); }

		SceneHandle RegisterScene(IScene* newScene)
		{
			Scenes.push_back(newScene);
			return Scenes.size() - 1;
		}

		void SetCurrenScene(SceneHandle scene)
		{
			CurrentScene = scene;
		}

		void Tick(f32 Delta);
	private:
		SceneHandle CurrentScene = SceneHandle::Zero;
		darray<Core::Memory::UniquePtr<IScene>> Scenes;
	};
}