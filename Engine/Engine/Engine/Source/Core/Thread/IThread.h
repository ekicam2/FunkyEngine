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

			private:
				str Name;
				Thread::Type Type;
			};
		}
	}
}