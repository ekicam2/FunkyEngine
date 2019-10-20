#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define FUNKY_WINDOWS
#endif

#include "Core/Thread/IThread.h"

#ifdef FUNKY_WINDOWS
#include "Core/Platform/Windows/FunkyWindows.h"
#endif

namespace Funky
{
	namespace Core
	{
		namespace Platform
		{
			Thread::IThread*	CreateThread(str const& Name, Thread::Type ThreadType);
			void				DestroyThread(Thread::IThread* ThreadToDestroy);
		}
	}
}