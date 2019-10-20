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
				using ThreadCreationDesc = map<Thread::Type, u16>;

				ThreadPool(ThreadCreationDesc const& CreationDesc);

			private:
				darray<IThread*> AllThreads;
			};

		}
	}
}