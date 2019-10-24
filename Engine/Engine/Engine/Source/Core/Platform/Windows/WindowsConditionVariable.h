#pragma once

#include "Core/Thread/IConditionVariable.h"
#include "Core/Platform/Platform.h"

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class WindowsConditionVariable final : public IConditionVariable
			{
			public:
				WindowsConditionVariable();
				~WindowsConditionVariable();
				virtual void Sleep(class IMutex* Mutex) override;
				virtual void Wake() override;
			private:
				CONDITION_VARIABLE Handle;
			};
		}
	}
}