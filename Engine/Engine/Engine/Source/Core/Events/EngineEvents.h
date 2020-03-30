#pragma once

#include "BasicTypes.h"
#include "Math/Math.h"
#include "Core/Events/Event.h"

namespace Funky
{
	extern Core::Event<void(Math::Vec2u)> OnViewportResized;
}