#pragma once

#include "BasicTypes.h"
#include "Core/String.h"

namespace Funky
{
	namespace Core
	{
		namespace Thread
		{
			enum class Type : u8
			{
				Any,
				Main,
				Rendering,
				Worker
			};

			class IThread
			{
			public:
				IThread(str const& Name, Thread::Type ThreadType);
				virtual ~IThread() = default;

				virtual i32 Run() = 0;

				static IThread* CreateThread(str const& Name, Funky::Core::Thread::Type ThreadType);

			private:
				str Name;
				Thread::Type Type;
			};
		}
	}
}