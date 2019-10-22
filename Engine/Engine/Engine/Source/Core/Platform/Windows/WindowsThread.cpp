#include "WindowsThread.h"

Funky::Core::Thread::IThread* Funky::Core::Thread::IThread::CreateThread(str const& Name, Funky::Core::Thread::Type ThreadType)
{
	WindowsThread* NewThread = new Funky::Core::Thread::WindowsThread(Name, ThreadType);
	HANDLE Handle = ::CreateThread(NULL, 0, WinThreadFunc, NewThread, 0, NULL);
	NewThread->SetHandle(Handle);
	return NewThread;
}


DWORD WinThreadFunc(LPVOID lpThreadParameter)
{
	return ((Funky::Core::Thread::WindowsThread*)(lpThreadParameter))->Run();
}

Funky::Core::Thread::WindowsThread::WindowsThread(str const& Name, Thread::Type ThreadType)
	: IThread(Name, ThreadType)
{
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
	return 0;
}

