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
				virtual void Process() = 0;
				virtual void PostProcess() {}

				FORCEINLINE Thread::Group GetThreadToRunOn() const;
				bool CanRunOnThread(Thread::Group InThreadType) const;

			protected:
				void SetThreadGroup(Thread::Group Type);
			private:
				Thread::Group ThreadToRunOn = Thread::Group::Any;
			};
		}
	}
}