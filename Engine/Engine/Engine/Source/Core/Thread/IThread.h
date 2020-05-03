#pragma once

#include "BasicTypes.h"
#include "Core/String.h"

#include "Core/Thread/IConditionVariable.h"
#include "Core/Thread/IMutex.h"

namespace Funky
{
	namespace Core
	{
		namespace Task
		{
			class ITask;
		}

		namespace Thread
		{
			enum class EGroup : u8
			{
				Any,
				Main,
				Rendering,
				Worker
			};

			class IThread
			{
			public:
				virtual ~IThread() = default;
				static IThread* CreateThread(Str const& Name, Funky::Core::Thread::EGroup ThreadType);

				virtual i32 Run() = 0;
				virtual void WaitForTask() = 0;

				bool IsWaitingForTask() const;
				void AssignTask(Task::ITask* TaskToAssign);

				FORCEINLINE Thread::EGroup GetType() const;

			protected:
				IThread(Str const& Name, Thread::EGroup ThreadType);

				Str Name;
				Thread::EGroup Type;

				mutable IMutex* StateMutex;
				bool IsWaitingForTaskState;

				mutable IMutex* TaskMutex;
				IConditionVariable* TaskReadyToProcess;
				Task::ITask* TaskToProcess;
			};
		}
	}
}
FORCEINLINE Funky::Core::Thread::EGroup Funky::Core::Thread::IThread::GetType() const
{
	return Type;
}