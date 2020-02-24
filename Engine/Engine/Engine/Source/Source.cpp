#include "Macros.h"
#include "FunkyEngine.h"
#include "LogMacros.h"

#define ENGINE

#ifdef MEMORY_TEST
#include <fstream>
#include <iterator>
#include <vector>
#include <locale>

#include "Core/Memory/MemoryPool.h"


struct Tester10MB
{
	byte __unused[10 * 1024 * 1024];
};
static_assert(sizeof(Tester10MB) == 10 * 1024 * 1024);


void DebugMemoryPool()
{
	BREAK();

	Funky::Core::Memory::MemoryPool MP;
	if (!MP.InitMemoryPool(512 * 1024 * 1024))
	BREAK();
	BREAK();


	Tester10MB* pTester = MP.Alloc<Tester10MB>();
	(void)pTester;
	BREAK();


	i32 allocations = std::rand() % 100;
	darray<void*> AllocatedDumbs;
	for (i32 i = 0; i < allocations; ++i)
	{
		AllocatedDumbs.push_back((void*)MP.Alloc<Tester10MB>());
	}

	do
	{
		i32 randomID = std::rand() % AllocatedDumbs.size();
		MP.Free(AllocatedDumbs[randomID]);
	} while (AllocatedDumbs.size() == 0);

	MP.Free(pTester);
	BREAK();

}

int main()
{
	// Something is wrong
	// im not able to deallocate all memory ive allocated!
	DebugMemoryPool();

	return 0;
	
}
#elif defined(THREAD_TEST)

#include <chrono>
#include <thread>

#include "Core/Platform/Platform.h"
#include "Core/Tasks/ITask.h"
#include "Core/Tasks/TaskManager.h"
#include "Core/Thread/ThreadPool.h"

class TestTask : public Funky::Core::Task::ITask
{
public:
	TestTask() : ITask(Funky::Core::Thread::Group::Any) {}
	void Process()
	{
		using namespace std::chrono_literals;
		std::cout << "Processing task" << std::endl;
		std::this_thread::sleep_for(2s);
		std::cout << "Processing task completed" << std::endl;
	}
};

int main()
{
	Funky::Core::Thread::ThreadPool ThreadPool({ {Funky::Core::Thread::Group::Worker, (u16)5u} });
	Funky::Core::Task::TaskManager TaskManager(&ThreadPool);
	
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	
	while(TaskManager.GetTasksCount() > 0)
		TaskManager.Tick();

	puts("Press enter to stop...");
	getchar();

	return 0;
}
#elif defined(ENGINE)
int main(i32 argc, char** argv)
{
	INIT_LOG();

	LOG("Begin cvars");
	for (i32 i = 0; i < argc; ++i)
	{
		std::string ar = argv[i];
		LOG(i, ": ", ar);
	}
	LOG("End cvars");

	const i32 len = 1024;
	charx pBuf[len];
	i32 bytes = GetModuleFileName(NULL, pBuf, len);
	if (bytes > 0)
	{
		LOG(pBuf);
	}

	Funky::FunkyEngine Engine;
	if (Engine.Init())
		Engine.Run();

	if (!Engine.Shutdown())
		LOG_ERROR("Error while cleaning up.");
	

	LOG("Waiting for input after close.");
	// wait with console
	getchar();

	return 0;
}
#endif
