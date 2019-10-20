#pragma once

#include "Core/Thread/IThread.h"
#include "Core/Platform/Windows/FunkyWindows.h"


DWORD WinThreadFunc(LPVOID lpThreadParameter);

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class WindowsThread : public IThread
			{
			public:
				WindowsThread(HANDLE Handle, str const& Name, Thread::Type ThreadType);
				~WindowsThread();

				FORCEINLINE HANDLE GetHandle() const { return Handle; }

			private:
				HANDLE Handle;
			};

		}
	}
}