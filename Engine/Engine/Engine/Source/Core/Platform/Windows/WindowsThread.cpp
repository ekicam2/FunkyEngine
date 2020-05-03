#include "WindowsThread.h"

#include "Core/Tasks/ITask.h"
#include "Core/Tasks/TaskManager.h"

Funky::Core::Thread::IThread* Funky::Core::Thread::IThread::CreateThread(Str const& Name, Funky::Core::Thread::EGroup ThreadType)
{
	WindowsThread* NewThread = new Funky::Core::Thread::WindowsThread(Name, ThreadType);
	HANDLE Handle = ::CreateThread(NULL, 0, WinThreadFunc, NewThread, 0, NULL);
	NewThread->SetHandle(Handle);
	switch (ThreadType)
	{
	case EGroup::Worker:
		SetThreadDescription(NewThread->GetHandle(), L"WorkerThread");
		break;
	case EGroup::Rendering:
		SetThreadDescription(NewThread->GetHandle(), L"RenderingThread");
		break;
	case EGroup::Any:
	case EGroup::Main:
		// do nothing
		break;
	}
	return NewThread;
}


DWORD WinThreadFunc(LPVOID lpThreadParameter)
{
	return ((Funky::Core::Thread::WindowsThread*)(lpThreadParameter))->Run();
}

Funky::Core::Thread::WindowsThread::WindowsThread(Str const& Name, Thread::EGroup ThreadType)
	: IThread(Name, ThreadType)
{
	TaskReadyToProcess = IConditionVariable::Create();
}

Funky::Core::Thread::WindowsThread::~WindowsThread()
{
	::CloseHandle(GetHandle());
}

void Funky::Core::Thread::WindowsThread::SetHandle(HANDLE NewHandle)
{
	if (Handle == NULL) 
		Handle = NewHandle;
}

i32 Funky::Core::Thread::WindowsThread::Run()
{
	while (true)
	{
		WaitForTask();

		TaskToProcess->Process();
		TaskToProcess->PostProcess();

		delete TaskToProcess;
		TaskToProcess = nullptr;

		MutexScopeGuard ScopeGuard1(StateMutex);
		IsWaitingForTaskState = true;

	}

	return 0;
}

void Funky::Core::Thread::WindowsThread::WaitForTask()
{
	while (TaskToProcess == nullptr)
		TaskReadyToProcess->Sleep(TaskMutex);
}

