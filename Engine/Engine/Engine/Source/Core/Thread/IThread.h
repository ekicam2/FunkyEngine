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
			enum class Type : u8
			{
				Any,
				Main,
				Rendering,
				Worker
			};

			class IThread
			{
			public:
				static IThread* CreateThread(str const& Name, Funky::Core::Thread::Type ThreadType);

				virtual i32 Run() = 0;
				virtual void WaitForTask() = 0;

				bool IsWaitingForTask() const;
				void AssignTask(Task::ITask* TaskToAssign);

				FORCEINLINE Thread::Type GetType() const;

			protected:
				IThread(str const& Name, Thread::Type ThreadType);
				virtual ~IThread() = default;

				str Name;
				Thread::Type Type;

				mutable IMutex* StateMutex;
				bool IsWaitingForTaskState;

				mutable IMutex* TaskMutex;
				IConditionVariable* TaskReadyToProcess;
				Task::ITask* TaskToProcess;
			};
		}
	}
}
FORCEINLINE Funky::Core::Thread::Type Funky::Core::Thread::IThread::GetType() const
{
	return Type;
}