#include "WindowsThread.h"

DWORD WinThreadFunc([[maybe_unused]] LPVOID lpThreadParameter)
{
	return 0;
}

Funky::Core::Thread::WindowsThread::WindowsThread(HANDLE _Handle, str const& Name, Thread::Type ThreadType)
	: IThread(Name, ThreadType)
{
	Handle = _Handle;
}

Funky::Core::Thread::WindowsThread::~WindowsThread()
{

}

