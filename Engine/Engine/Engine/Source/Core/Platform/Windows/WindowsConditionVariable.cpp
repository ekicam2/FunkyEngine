#include "WindowsConditionVariable.h"

#include "WindowsMutex.h"

Funky::Core::Thread::IConditionVariable* Funky::Core::Thread::IConditionVariable::Create()
{
	return new WindowsConditionVariable();
}


Funky::Core::Thread::WindowsConditionVariable::WindowsConditionVariable()
{
	::InitializeConditionVariable(&Handle);
}

Funky::Core::Thread::WindowsConditionVariable::~WindowsConditionVariable()
{

}

void Funky::Core::Thread::WindowsConditionVariable::Sleep(IMutex* Mutex)
{
	auto MutexHandle = static_cast<WindowsMutex*>(Mutex)->GetHandle();
	::SleepConditionVariableCS(&Handle, &MutexHandle, INFINITE);
}

void Funky::Core::Thread::WindowsConditionVariable::Wake()
{
	::WakeConditionVariable(&Handle);
}
