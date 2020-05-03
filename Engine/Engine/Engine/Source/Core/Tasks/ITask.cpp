#include "ITask.h"


Funky::Core::Task::ITask::ITask()
{
}

FORCEINLINE Funky::Core::Thread::EGroup  Funky::Core::Task::ITask::GetThreadToRunOn() const
{
	return ThreadToRunOn;
}

bool Funky::Core::Task::ITask::CanRunOnThread(Thread::EGroup InThreadType) const
{
	if (ThreadToRunOn == Thread::EGroup::Any) 
		return true;
	else if (ThreadToRunOn == InThreadType) 
		return true;

	return false;
}

void Funky::Core::Task::ITask::SetThreadGroup(Thread::EGroup Type)
{
	ThreadToRunOn = Type;
}
