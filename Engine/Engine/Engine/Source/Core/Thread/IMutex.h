#pragma once

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class IMutex
			{
				/* If mutex is free return true and lock mutex otherwise return false and suspend execution. */
				virtual bool Wait() = 0;
				virtual void Free() = 0;
			};
		}
	}
}