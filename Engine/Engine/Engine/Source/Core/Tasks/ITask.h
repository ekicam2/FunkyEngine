#pragma once

#include "BasicTypes.h"
#include "Core/String.h"

#include "Core/Thread/IThread.h"

namespace Funky
{
	namespace Core
	{
		namespace Task
		{
			class ITask 
			{
			public:
				ITask(Thread::Type Type);
				virtual void Run() = 0;

				FORCEINLINE Thread::Type GetThreadToRunOn() const { return ThreadToRunOn; }
			private:
				Thread::Type ThreadToRunOn;
			};
		}
	}
}