#pragma once

#include "Core/Containers.h"
#include "Core/Thread/IThread.h"
#include "Core/Platform/Platform.h"

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class ThreadPool
			{
			public:
				using ThreadCreationDesc = map<Thread::EGroup, u16>;

				ThreadPool(ThreadCreationDesc const& CreationDesc);

				IThread* GetIdleThread(Funky::Core::Thread::EGroup Type);

			private:
				darray<IThread*> AllThreads;
			};

		}
	}
}