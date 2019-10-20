#include "ITask.h"


Funky::Core::Task::ITask::ITask(Thread::Type _Type)
	: ThreadToRunOn(_Type)
{
}
