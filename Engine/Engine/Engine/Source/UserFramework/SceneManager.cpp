#include "SceneManager.h"
#include "UserFramework/Scene.h"


Funky::SceneManager::SceneManager()
{
	Scenes.push_back(new Scene());
}

void Funky::SceneManager::Tick(f32 Delta)
{
	GetCurrentScene()->Tick(Delta);
}


