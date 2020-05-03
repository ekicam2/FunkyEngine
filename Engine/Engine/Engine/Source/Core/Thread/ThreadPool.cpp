#include "ThreadPool.h"


Funky::Core::Thread::ThreadPool::ThreadPool(ThreadCreationDesc const& CreationDesc)
{
	u32 ThreadsCount = 0u;
	for (auto& Desc : CreationDesc)
		ThreadsCount += Desc.second;

	AllThreads.reserve(ThreadsCount);

	for (auto& Desc : CreationDesc)
	{
		for (u16 i = 0; i < Desc.second; ++i)
			AllThreads.push_back(Funky::Core::Thread::IThread::CreateThread("Unnamed", Desc.first));
	}
}

Funky::Core::Thread::IThread* Funky::Core::Thread::ThreadPool::GetIdleThread(Funky::Core::Thread::EGroup Type)
{
	for (auto& Thread : AllThreads)
	{
		if ((Type == EGroup::Any || Thread->GetType() == Type) && Thread->IsWaitingForTask())
			return Thread;
	}

	return nullptr;
}
