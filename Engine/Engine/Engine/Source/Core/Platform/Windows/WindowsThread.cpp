#include "WindowsThread.h"

#include "Core/Tasks/ITask.h"
#include "Core/Tasks/TaskManager.h"

Funky::Core::Thread::IThread* Funky::Core::Thread::IThread::CreateThread(str const& Name, Funky::Core::Thread::Group ThreadType)
{
	WindowsThread* NewThread = new Funky::Core::Thread::WindowsThread(Name, ThreadType);
	HANDLE Handle = ::CreateThread(NULL, 0, WinThreadFunc, NewThread, 0, NULL);
	NewThread->SetHandle(Handle);
	switch (ThreadType)
	{
	case Group::Worker:
		SetThreadDescription(NewThread->GetHandle(), L"WorkerThread");
		break;
	case Group::Rendering:
		SetThreadDescription(NewThread->GetHandle(), L"RenderingThread");
		break;
	}
	return NewThread;
}


DWORD WinThreadFunc(LPVOID lpThreadParameter)
{
	return ((Funky::Core::Thread::WindowsThread*)(lpThreadParameter))->Run();
}

Funky::Core::Thread::WindowsThread::WindowsThread(str const& Name, Thread::Group ThreadType)
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

		MutexScopeGuard ScopeGuard(StateMutex);
		IsWaitingForTaskState = true;

		delete TaskToProcess;
		TaskToProcess = nullptr;
	}

	return 0;
}

void Funky::Core::Thread::WindowsThread::WaitForTask()
{
	while (TaskToProcess == nullptr)
		TaskReadyToProcess->Sleep(TaskMutex);
}

