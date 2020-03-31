#pragma once

#include "BasicTypes.h"

#include <Memory.h>

namespace Funky
{
	namespace Core
	{
		namespace Memory
		{
			struct Utils
			{

				/** Convert Bytest to MB */
				template <typename T>
				static constexpr T BToMB(T InBytes) { return InBytes / ((T)1024 * (T)1024); }

				/** Convert Bytest to MB */
				template <typename T>
				static constexpr T MBToB(T InBytes) { return InBytes * ((T)1024 * (T)1024); }
			};
		}
	}
}


FORCEINLINE void MemSet(void* Dest, size Size, byte Data)
{
	::memset(Dest, Data, Size);
}

FORCEINLINE void ZeroMem(void* Dest, size Size)
{
	MemSet(Dest, Size, 0);
}

FORCEINLINE void Memcpy(void* Src, void* Dst, size Size)
{
	::memcpy(Dst, Src, Size);
}