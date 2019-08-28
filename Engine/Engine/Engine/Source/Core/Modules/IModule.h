#pragma once

#include "Core/String.h"

namespace Funky
{
	namespace Core
	{
		namespace Module
		{
			class IModule
			{
				str ModuleName;

				virtual bool Init() = 0;
				virtual void Shutdown() = 0;
			};
		}
	};
};