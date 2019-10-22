#pragma once

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class IMutex
			{
				friend class MutexScopeGuard;
				/* If mutex is free return true and lock mutex otherwise return false and suspend execution. */
				virtual bool Wait() = 0;
				virtual void Free() = 0;
			};

			class MutexScopeGuard final
			{
			public:
				MutexScopeGuard(IMutex* NewMutex) : Mutex(NewMutex) { Mutex->Wait(); }
				~MutexScopeGuard() { Mutex->Free(); }
			private:
				IMutex* Mutex;
			};
		}
	}
}