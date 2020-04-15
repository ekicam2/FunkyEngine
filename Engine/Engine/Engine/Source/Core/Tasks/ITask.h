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
				ITask();
				virtual ~ITask() {}
				virtual void Process() = 0;
				virtual void PostProcess() {}

				FORCEINLINE Thread::EGroup GetThreadToRunOn() const;
				bool CanRunOnThread(Thread::EGroup InThreadType) const;

			protected:
				void SetThreadGroup(Thread::EGroup Type);
			private:
				Thread::EGroup ThreadToRunOn = Thread::EGroup::Any;
			};
		}
	}
}