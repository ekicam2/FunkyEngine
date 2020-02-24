#pragma once

#include "Macros.h"
#include "BasicTypes.h"

#include "Core/Containers.h"
#include "Core/Thread/IThread.h"

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class IMutex;
			class ThreadPool;
		}

		namespace Task
		{
			class ITask;

			class TaskManager
			{
			public:
				TaskManager(Thread::ThreadPool* ThreadPool);
				~TaskManager();

				/* WARNING!! wait free function! */
				void EnqueueTask(ITask* NewTask);
				/* WARNING!! wait free function! */
				ITask* DequeueTask(Thread::Group RequiredThreadToRunOn);
				
				/* Mutex implementation */
				void EnqueueTaskSafe(ITask* NewTask);
				/* Mutex implementation */
				ITask* DequeueTaskSafe();

				void Tick();

				/* Aproximated value, may not be correct. */
				FORCEINLINE u64 GetTasksCount() const { return TaskQueue.size(); }
				FORCEINLINE Thread::IMutex* GetQueueMutex() const { return QueueMutex; };


			private:
				Thread::ThreadPool* ThreadPool;

				Thread::IMutex* QueueMutex;
				darray<ITask*> TaskQueue;
			};
		}
	}
}
