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
				WindowsThread(Str const& Name, Thread::EGroup ThreadType);
				~WindowsThread();

				void SetHandle(HANDLE NewHandle);
				FORCEINLINE HANDLE GetHandle() const { return Handle; }

				virtual i32 Run() override;
				virtual void WaitForTask() override;

			private:
				HANDLE Handle = NULL;
			};

		}
	}
}