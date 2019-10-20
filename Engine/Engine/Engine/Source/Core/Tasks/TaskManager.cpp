#include "TaskManager.h"



void Funky::Core::Task::TaskManager::EnqueueTask(ITask* NewTask)
{
	TaskQueue.push_back(NewTask);
}

Funky::Core::Task::ITask* Funky::Core::Task::TaskManager::DequeueTask()
{
	if (TaskQueue.size() == 0u) return nullptr;

	ITask* Ret = TaskQueue[0];
	TaskQueue.erase(TaskQueue.begin());
	return Ret;
}
