#include "ITask.h"


Funky::Core::Task::ITask::ITask()
{
}

FORCEINLINE Funky::Core::Thread::Group  Funky::Core::Task::ITask::GetThreadToRunOn() const
{
	return ThreadToRunOn;
}

bool Funky::Core::Task::ITask::CanRunOnThread(Thread::Group InThreadType) const
{
	if (ThreadToRunOn == Thread::Group::Any) 
		return true;
	else if (ThreadToRunOn == InThreadType) 
		return true;

	return false;
}

void Funky::Core::Task::ITask::SetThreadGroup(Thread::Group Type)
{
	ThreadToRunOn = Type;
}
