#pragma once

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			class IConditionVariable
			{
			public:
				static IConditionVariable* Create();

				virtual void Sleep(class IMutex* Mutex) = 0;
				virtual void Wake() = 0;

			protected:
				IConditionVariable() = default;
				virtual ~IConditionVariable() = default;
			};
		}
	}
}