#include "Macros.h"
#include "Engine.h"

/**
 *	Valid defines
 * ENGINE
 *  THREADED_RENDERING (unusable)
 * MEMORY_TEST
 * THREAD_TEST
 * STRING_TEST
 */


#if !(defined(MEMORY_TEST) || defined(THREAD_TEST) || defined(STRING_TEST))
	#define ENGINE
#endif

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
#endif 
#ifdef THREAD_TEST

#include <chrono>
#include <thread>

#include "Core/Platform/Platform.h"
#include "Core/Tasks/ITask.h"
#include "Core/Tasks/TaskManager.h"
#include "Core/Thread/ThreadPool.h"

class TestTask : public Funky::Core::Task::ITask
{
public:
	TestTask() : ITask(Funky::Core::Thread::EGroup::Any) {}
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
	Funky::Core::Thread::ThreadPool ThreadPool({ {Funky::Core::Thread::EGroup::Worker, (u16)5u} });
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

#endif
#ifdef STRING_TEST
#include "Core/String.h"
#include "Templates.h"
#include "DebugMacros.h"

int main()//(i32 argc, char** argv)
{
	INIT_LOG();
	
	Str s1 = "warsadasf";
	Str s2 = s1;
	ASSERT(s1 == s2, TEXT("Strings are not the same"));
	CHECK(s1.Length() == s2.Length());


	decltype(s1) s3 = Funky::Move(s1);
	ASSERT(s1 == s3, TEXT("Strings are not the same"));

	s1 = "123jfbiudsknfoidsniufnodsifnmoig";
	ASSERT(s1 != s3, TEXT("Strings are the same!"));

	decltype(s1) s4(s1.Length());
	s4 = Funky::Move(s1);
	CHECK(s1.Capacity() == 0);
	CHECK(s1.Length() == 0);
	CHECK(s4.Length() == StrLen(s4.GetBuffer()));


	Str s5 = "Control_Module_rough.jpg";
	CHECK(s5.EndsWith(".jpg"));
	CHECK(!s5.EndsWith(".jg2"));
	CHECK(!s5.EndsWith(".jpg2"));
	CHECK(s5.StartsWith("Control_"));
	CHECK(!s5.StartsWith("Control_2"));
	CHECK(!s5.StartsWith("Control_Control_Control_Control_Control_"));


	return 0;
}
#endif
#ifdef ENGINE


// clang
// -Wno-pragma-once-outside-header 

#if !defined(_WINDLL)

#ifdef _DEBUG
#include "Debug/EngineDebugScene.h"
#endif // _DEBUG

int main(i32 argc, char** argv)
{
	INIT_LOG();
	
	SetCurrentDirectory(
		TEXT("G:\\Engine\\Engine\\Engine\\Engine\\Work")
	);

	Funky::Engine::EngineInitDesc EngineInit = {};
	EngineInit.Argc = argc;
	EngineInit.Argv = argv;
	EngineInit.RequiredSubsystems = Funky::SubsystemBitmask::All;
#ifdef _DEBUG
	EngineInit.Initializer.RegisterLambda([](Funky::Engine* inEngine, bool succeeded)
		{
			if (succeeded)
			{
				auto sceneManager = inEngine->GetSceneManager();
				sceneManager->SetCurrenScene(sceneManager->RegisterScene(new Funky::EngineDebugScene()));
				sceneManager->GetCurrentScene()->Init();
			}
		});
#endif

	{

		Funky::Engine Engine;
		if (Engine.Init(&EngineInit))
			Engine.Run();
	
		if (!Engine.Shutdown())
			LOG_ERROR("Error while cleaning up.");
	}
	
	return 0;
}
#endif
#endif
