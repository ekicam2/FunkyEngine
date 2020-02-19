#pragma once

#include "Math/Vector3.h"
#include "Math/LinearColor.h"

struct Light
{
	Light()
		: Position(0.0f, 0.0f, 0.0f)
		, Color(0.0f, 1.0f, 0.0, 1.0f)
	{

	}

	Light(Funky::Math::Vec3f vPosition, Funky::Math::LinearColor LightColor)
		: Position(vPosition)
		, Color(LightColor)
	{

	}

	Funky::Math::LinearColor Color;
	Funky::Math::Vec3f Position;

private:
	float padding = 0.0987654f;
};