#pragma once

#include "Core/IIOSystem.h"

namespace Funky
{
	namespace Core
	{
		namespace IO
		{
			class WindowsIOSystem final : public IIOSystem
			{
				virtual ~WindowsIOSystem() {}
				virtual void Update();
			};
		}
	}
}