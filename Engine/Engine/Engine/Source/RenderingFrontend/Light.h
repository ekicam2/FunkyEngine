#pragma once

#include "Math/Vector3.h"
#include "Math/LinearColor.h"

namespace Funky
{
	struct Light
	{
		Light()
			: Position(0.0f, 0.0f, 0.0f)
			, Color(0.0f, 1.0f, 0.0, 1.0f)
		{

		}

		Light(Math::Vec3f vPosition, Math::LinearColor LightColor)
			: Position(vPosition)
			, Color(LightColor)
		{

		}

		Math::LinearColor Color;
		Math::Vec3f Position;

	private:
		float padding = 0.0987654f;
	};

}