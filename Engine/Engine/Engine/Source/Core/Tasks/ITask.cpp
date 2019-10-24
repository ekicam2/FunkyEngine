#include "ITask.h"


Funky::Core::Task::ITask::ITask(Thread::Type _Type)
	: ThreadToRunOn(_Type)
{
}

FORCEINLINE Funky::Core::Thread::Type  Funky::Core::Task::ITask::GetThreadToRunOn() const
{
	return ThreadToRunOn;
}
