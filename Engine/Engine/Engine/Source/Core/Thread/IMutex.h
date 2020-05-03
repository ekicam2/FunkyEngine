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
			public:
				static IMutex* Create();

			protected:
				IMutex() = default;
				virtual ~IMutex() = default;

				/* If mutex is free return true and lock mutex otherwise return false and suspend execution. */
				virtual void Wait() = 0;
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