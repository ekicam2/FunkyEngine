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
			public:
				FORCEINLINE CRITICAL_SECTION GetHandle() const;

				WindowsMutex();
				~WindowsMutex();
			private:

				virtual void Wait() override;
				virtual void Free() override;

				CRITICAL_SECTION Handle;
			}; 
		}
	}
}

FORCEINLINE CRITICAL_SECTION Funky::Core::Thread::WindowsMutex::GetHandle() const
{
	return Handle;
}