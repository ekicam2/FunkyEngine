#include "WindowsMutex.h"

#pragma push_macro("CreateMutex")
#undef CreateMutex

Funky::Core::Thread::IMutex* Funky::Core::Thread::IMutex::Create()
{
	return new WindowsMutex();
}

#pragma pop_macro("CreateMutex")

Funky::Core::Thread::WindowsMutex::WindowsMutex()
{
	::InitializeCriticalSection(&Handle);
}

Funky::Core::Thread::WindowsMutex::~WindowsMutex()
{
	::DeleteCriticalSection(&Handle);
}

void Funky::Core::Thread::WindowsMutex::Wait()
{
	::EnterCriticalSection(&Handle);
}

void Funky::Core::Thread::WindowsMutex::Free()
{
	::LeaveCriticalSection(&Handle);
}

