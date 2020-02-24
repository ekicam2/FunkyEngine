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
				using ThreadCreationDesc = map<Thread::Group, u16>;

				ThreadPool(ThreadCreationDesc const& CreationDesc);

				IThread* GetIdleThread(Funky::Core::Thread::Group Type);

			private:
				darray<IThread*> AllThreads;
			};

		}
	}
}