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
	void Run()
	{
		std::cout << "Processing task" << std::endl;
	}
};

int main()
{
	Funky::Core::Task::TaskManager TaskManager;
	Funky::Core::Thread::ThreadPool ThreadPool({ {Funky::Core::Thread::Type::Worker, (u16)5u} });
	
	auto TT = new TestTask();
	TaskManager.EnqueueTask((Funky::Core::Task::ITask*)TT);
	TaskManager.EnqueueTask((Funky::Core::Task::ITask*)TT);
	TaskManager.EnqueueTask((Funky::Core::Task::ITask*)TT);
	TaskManager.EnqueueTask((Funky::Core::Task::ITask*)TT);
	TaskManager.EnqueueTask((Funky::Core::Task::ITask*)TT);

	return 0;
}
#else
int main()
{
	Funky::FunkyEngine Engine;
	if (Engine.Init())
		Engine.Run();

	Engine.Shutdown();

	return 0;
}
#endif