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
			enum class Group : u8
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
				static IThread* CreateThread(str const& Name, Funky::Core::Thread::Group ThreadType);

				virtual i32 Run() = 0;
				virtual void WaitForTask() = 0;

				bool IsWaitingForTask() const;
				void AssignTask(Task::ITask* TaskToAssign);

				FORCEINLINE Thread::Group GetType() const;

			protected:
				IThread(str const& Name, Thread::Group ThreadType);

				str Name;
				Thread::Group Type;

				mutable IMutex* StateMutex;
				bool IsWaitingForTaskState;

				mutable IMutex* TaskMutex;
				IConditionVariable* TaskReadyToProcess;
				Task::ITask* TaskToProcess;
			};
		}
	}
}
FORCEINLINE Funky::Core::Thread::Group Funky::Core::Thread::IThread::GetType() const
{
	return Type;
}