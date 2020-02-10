#include "TaskManager.h"

#include "Core/Tasks/ITask.h"
#include "Core/Thread/IMutex.h"

#include "Core/Thread/ThreadPool.h"

Funky::Core::Task::TaskManager::TaskManager(Thread::ThreadPool* _ThreadPool)
	: ThreadPool(_ThreadPool)
	, QueueMutex(Funky::Core::Thread::IMutex::Create())
{

}

Funky::Core::Task::TaskManager::~TaskManager()
{
	for (auto& t : TaskQueue) delete t;
}

void Funky::Core::Task::TaskManager::EnqueueTask(ITask* NewTask)
{
	TaskQueue.push_back(NewTask);
}

Funky::Core::Task::ITask* Funky::Core::Task::TaskManager::DequeueTask(Thread::Type RequiredThreadToRunOn)
{
	if (TaskQueue.size() == 0u) return nullptr;

	ITask* Ret = nullptr;

	for (u64 i = 0u; i < TaskQueue.size(); ++i)
	{
		if (TaskQueue[i]->CanRunOnThread(RequiredThreadToRunOn))
		{
			Ret = TaskQueue[i];
			TaskQueue.erase(TaskQueue.begin() + i);
		}
	}

	return Ret;
}

void Funky::Core::Task::TaskManager::EnqueueTaskSafe(ITask* NewTask)
{
	Thread::MutexScopeGuard ScopeGuard(QueueMutex);
	TaskQueue.push_back(NewTask);
}

Funky::Core::Task::ITask* Funky::Core::Task::TaskManager::DequeueTaskSafe()
{
	Thread::MutexScopeGuard ScopeGuard(QueueMutex);

	if (TaskQueue.size() == 0u) return nullptr;

	ITask* Ret = TaskQueue[0];
	TaskQueue.erase(TaskQueue.begin());
	return Ret;
}

void Funky::Core::Task::TaskManager::Tick()
{
	Thread::MutexScopeGuard ScopeGuard(QueueMutex);
	
	//always try to dispatch worker job first	
	Funky::Core::Thread::IThread* Thread = ThreadPool->GetIdleThread(Funky::Core::Thread::Type::Worker);
	if (Thread != nullptr)
	{
		ITask* Task = DequeueTask(Thread->GetType());
		if (Task != nullptr)
		{
			Thread->AssignTask(Task);
		}
	}

	Thread = ThreadPool->GetIdleThread(Funky::Core::Thread::Type::Rendering);
	if (Thread != nullptr)
	{
		ITask* Task = DequeueTask(Thread->GetType());
		if (Task != nullptr)
		{
			Thread->AssignTask(Task);
		}
	}
}
