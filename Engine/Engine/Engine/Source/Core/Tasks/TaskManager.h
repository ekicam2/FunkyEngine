#pragma once

#include "Macros.h"
#include "BasicTypes.h"

#include "Core/Containers.h"

namespace Funky
{
	namespace Core
	{
		namespace Task
		{
			class ITask;

			class TaskManager
			{
			public:
				/* WARNING!! wait free function! */
				void EnqueueTask(ITask* NewTask);
				/* WARNING!! wait free function! */
				ITask* DequeueTask();
				
				/* Mutex implementation */
				void EnqueueTaskSafe(ITask* NewTask);
				/* Mutex implementation */
				ITask* DequeueTaskSafe();

				/* Aproximated value, may not be correct. */
				FORCEINLINE u64 GetTasksCount() const { return TaskQueue.size(); }

			private:
				darray<ITask*> TaskQueue;
			};
		}
	}
}
