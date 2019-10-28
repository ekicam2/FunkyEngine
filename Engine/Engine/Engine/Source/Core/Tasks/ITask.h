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
				virtual void Process() = 0;
				virtual void PostProcess() {}

				FORCEINLINE Thread::Type GetThreadToRunOn() const;
			private:
				Thread::Type ThreadToRunOn;
			};
		}
	}
}