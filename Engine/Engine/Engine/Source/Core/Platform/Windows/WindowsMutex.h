#pragma once

#include "Core/Thread/IMutex.h"
#include "Core/Platform/Platform.h"

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class WindowsMutex final : public IMutex
			{
				WindowsMutex();
				~WindowsMutex();

				virtual bool Wait() override;
				virtual void Free() override;


				LPCRITICAL_SECTION Handle;
			}; 

			WindowsMutex::WindowsMutex()
			{
				::InitializeCriticalSection(Handle);
			}

			WindowsMutex::~WindowsMutex()
			{
				::DeleteCriticalSection(Handle);
			}

			bool WindowsMutex::Wait()
			{
				::EnterCriticalSection(Handle);
			}

			void WindowsMutex::Free()
			{
				::LeaveCriticalSection(Handle);
			}

		}
	}
}