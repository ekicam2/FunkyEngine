#include "IThread.h"

#include "Core/Thread/IMutex.h"

Funky::Core::Thread::IThread::IThread(str const& Name, Thread::Type ThreadType)
	: Name(Name)
	, Type(ThreadType)
	, StateMutex(IMutex::Create())
	, IsWaitingForTaskState(true)
	, TaskMutex(IMutex::Create())
	, TaskReadyToProcess(IConditionVariable::Create())
	, TaskToProcess(nullptr)
{

}

bool Funky::Core::Thread::IThread::IsWaitingForTask() const
{
	MutexScopeGuard ScopeGuard(StateMutex);
	return IsWaitingForTaskState;
}

void Funky::Core::Thread::IThread::AssignTask(Task::ITask* TaskToAssign)
{
	{
		MutexScopeGuard ScopeGuard(StateMutex);
		IsWaitingForTaskState = false;
	}

	MutexScopeGuard ScopeGuard(TaskMutex);
	TaskToProcess = TaskToAssign;
	TaskReadyToProcess->Wake();
}
