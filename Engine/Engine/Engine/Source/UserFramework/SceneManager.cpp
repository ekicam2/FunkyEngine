#include "SceneManager.h"
#include "UserFramework/Scene.h"

void Funky::SceneManager::Tick(f32 Delta)
{
	GetCurrentScene()->Tick(Delta);
}


