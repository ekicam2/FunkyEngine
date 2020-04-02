#pragma once

#include <string>
typedef std::string str;


size Strlen(char const * InCStr)
{
	return ::strlen(InCStr);
}

#include "Core/Memory/UniquePtr.h"

#define SMALL_STRING 16

template <typename T>
class Str final
{
public:
	Str(T const * const InCStr)
	{
		StringLength = Strlen(InCStr);

		if (SMALL_STRING < StringLength)
		{
			MemCpy(InCStr, SmallValue, StringLength);
			SmallValue[StringLength + 1] = '\000';
		}
		else
		{
			Value.Reset(new T[StringLength + 1]);
			MemCpy(InCStr, Value.Get(), StringLength);
			Value.Get()[StringLength + 1] = '\000';
		}
	}

	Str(Str const & Other)
	{
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
			SmallValue[StringLength + 1] = '\000';
		}
		else
		{
			MemCpy(Other.Value.Get(), Value.Get(), StringLength);
			Value.Get()[StringLength + 1] = '\000';
		}
	}

	Str(Str&& Other)
	{
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
			SmallValue[StringLength + 1] = '\000';
		}
		else
		{
			Value = Move(Other.Value);
		}
	}

	void operator=(Str const & Other)
	{
		const size PrevLength = StringLength;
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
		}
		else
		{
			if (PrevLength > StringLength)
				Value.Reset(new T[StringLength + 1]);

			MemCpy(Other.Value.Get(), Other.Value.Get(), StringLength);
			SmallValue[StringLength + 1] = '\000';
		}
	}

	void operator=(Str&& Other)
	{
		const size PrevLength = StringLength;
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.InCStr, Other.SmallValue, StringLength);
		}
		else
		{
			Value = Move(Other.Value);
		}
	}

	bool operator==(Str const & Other) const
	{
		return StrCmp() == 0;
	}

private:
	size StringLength = 0u;

	T SmallValue[SMALL_STRING];
	Core::Memory::UniquePtr<T[]> Value;
};