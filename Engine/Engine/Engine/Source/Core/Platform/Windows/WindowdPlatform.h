#pragma once 

#include "Core/Platform/Platform.h"
#include "Core/Platform/Windows/WindowsThread.h"

#ifdef FUNKY_WINDOWS

Funky::Core::Thread::IThread* Funky::Core::Platform::CreateThread(str const& Name, Funky::Core::Thread::Type ThreadType)
{
	HANDLE Handle = ::CreateThread(NULL, 0, WinThreadFunc, nullptr, 0, NULL);
	return new Funky::Core::Thread::WindowsThread(Handle, Name, ThreadType);
}

void Funky::Core::Platform::DestroyThread(Funky::Core::Thread::IThread* ThreadToDestroy)
{
	auto WinThread = static_cast<Funky::Core::Thread::WindowsThread*>(ThreadToDestroy);
	::CloseHandle(WinThread->GetHandle());
}

#endif