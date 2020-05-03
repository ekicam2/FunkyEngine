#pragma once

#include "fk_type_defs.h"
#include <memory.h>

namespace fk
{
	// Bigger task
	// 	struct MemHeader
	// 	{
	// 		u64 bytesWritten;
	// 		u64 chunkSize;
	// 		byte* data;
	// 	};
	// 
	// 	MemHeader alloc(u64 bytes);
	// 	MemHeader allocFilled(u64 bytes, byte data = 0);
	// 
	// 	void free(MemHeader const & data);

	void memCpy(void const * const src, void* dest, u64 bytes);
	void memSet(void* dest, u64 bytes, byte value = 0);
}

#ifdef FK_IMPLEMENTATION

#if FK_UNKNOWN
void __memCpy(void* src, void* dest, u64 bytes)
{
	byte* read = (byte*)src;
	byte* write = (byte*)dest;

	if (read == write)
	{
		return;
	}

	const byte* copyEnd = read + bytes;
	while (copyEnd != read)
	{
		*read++ = *write++;
	}
}
#endif

void fk::memCpy(void const * const src, void* dest, const u64 bytes)
{
#if FK_UNKNOWN
	__memCpy(src, dest, bytes);
#else
	::memcpy(dest, src, bytes);
#endif
}

#if FK_UNKNOWN
void __memSet(void* dest, u64 bytes, byte value)
{
	byte* write = (byte*)dest;

	if (bytes == 0)
	{
		return;
	}

	const byte* writeEnd = write + bytes;
	while (writeEnd != write)
	{
		*write++ = value;
	}
}
#endif

void fk::memSet(void* dest, u64 bytes, byte value)
{
#if FK_UNKNOWN
	__memSet(dest, bytes, value);
#else
	::memset(dest, value, bytes);
#endif
}
#endif // FK_IMPLEMENTATION