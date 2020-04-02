#pragma once

#include "String.h"
#include "BasicTypes.h"
#include "3rd/MurmurHash3.h"

namespace Funky
{
	struct Hash128
	{
		u64 v[2];
		FORCEINLINE bool operator==(Hash128 const& Other) { return v[0] == Other.v[0] && v[1] == Other.v[1]; }
	};

	/* It's best to use prime numbers. */
	template <u32 Seed = 17>
	Hash128 HashString(str const& InStr)
	{
		Hash128 Ret;
		MurmurHash3_x64_128(InStr.c_str(), (int)InStr.size(), Seed, Ret.v);
		return Ret;
	}
}