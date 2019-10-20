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
			AllThreads.push_back(Core::Platform::CreateThread("Unnamed", Desc.first));
	}
}
