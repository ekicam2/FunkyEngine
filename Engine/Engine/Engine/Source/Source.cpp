#include "FunkyEngine.h"

#define THREAD_TEST

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

#include "Core/Platform/Platform.h"
#include "Core/Tasks/ITask.h"
#include "Core/Tasks/TaskManager.h"
#include "Core/Thread/ThreadPool.h"

class TestTask : public Funky::Core::Task::ITask
{
public:
	TestTask() : ITask(Funky::Core::Thread::Type::Any) {}
	void Process()
	{
		std::cout << "Processing task" << std::endl;
	}
};

int main()
{
	Funky::Core::Thread::ThreadPool ThreadPool({ {Funky::Core::Thread::Type::Worker, (u16)5u} });
	Funky::Core::Task::TaskManager TaskManager(&ThreadPool);
	
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	TaskManager.EnqueueTaskSafe((Funky::Core::Task::ITask*)new TestTask());
	
	TaskManager.Tick();

	puts("Press enter to stop...");
	getchar();

	return 0;
}
#elif defined(ENGINE)
int main()
{
	Funky::FunkyEngine Engine;
	if (Engine.Init())
		Engine.Run();

	Engine.Shutdown();

	return 0;
}
#endif
