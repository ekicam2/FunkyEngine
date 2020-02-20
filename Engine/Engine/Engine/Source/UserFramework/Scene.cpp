#include "Scene.h"



void Funky::Scene::Init()
{
	LOG("Init Scene");
}

void Funky::Scene::Tick([[maybe_unused]] f32 Delta)
{
	LOG("SceneTick: ", Delta);
}
