#pragma once

#include "String.h"
#include "BasicTypes.h"
#include "3rd/MurmurHash3.h"

namespace Funky
{
	struct Hash128
	{
		u64 v[2];
		FORCEINLINE bool operator==(Hash128 const& Other) const { return v[0] == Other.v[0] && v[1] == Other.v[1]; }
	};

	/* It's best to use prime numbers. */
	template <u32 Seed = 17>
	Hash128 HashString(Str const& InStr)
	{
		Hash128 Ret;
		MurmurHash3_x64_128(InStr.GetBuffer(), (int)InStr.Length(), Seed, Ret.v);
		return Ret;
	}
}

namespace std {

	template <>
	struct hash<Funky::Hash128>
	{
		std::size_t operator()(const Funky::Hash128& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<u64>()(k.v[0])) ^ (hash<u64>()(k.v[1]) << 1));
		}
	};

}