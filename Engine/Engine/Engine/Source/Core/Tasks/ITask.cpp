#include "ITask.h"


Funky::Core::Task::ITask::ITask(Thread::Type _Type)
	: ThreadToRunOn(_Type)
{
}

FORCEINLINE Funky::Core::Thread::Type  Funky::Core::Task::ITask::GetThreadToRunOn() const
{
	return ThreadToRunOn;
}

bool Funky::Core::Task::ITask::CanRunOnThread(Thread::Type InThreadType) const
{
	if (ThreadToRunOn == Thread::Type::Any) 
		return true;
	else if (ThreadToRunOn == InThreadType) 
		return true;

	return false;
}
