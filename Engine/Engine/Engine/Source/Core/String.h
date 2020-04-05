#pragma once
#include <cstring>
#include "BasicTypes.h"
#include "Core/Memory/Memory.h"

FORCEINLINE size StrLen(char const * InCStr)
{
	return ::strlen(InCStr);
}

#include "Core/Memory/UniquePtr.h"

#define SMALL_STRING 16

template <typename T>
class String final
{
public:
	String(size RequiredSize = 0u)
	{
		if (RequiredSize > SMALL_STRING)
		{
			Allocate(RequiredSize + 1);
		}
	}

	String(T const * const InCStr, size Substring = 0u)
	{
		StringLength = StrLen(InCStr);

		const size CopyLength = Substring != 0u ? Substring : StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(InCStr, SmallValue, CopyLength);
			SmallValue[CopyLength] = '\0';
		}
		else
		{
			Allocate(StringLength + 1);
			MemCpy(InCStr, Value.Get(), CopyLength);
			Value.Get()[CopyLength] = '\0';
		}
	}

	String(String const & Other)
	{
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
			SmallValue[StringLength] = '\0';
		}
		else
		{
			Allocate(StringLength + 1);
			MemCpy(Other.Value.Get(), Value.Get(), StringLength);
			Value.Get()[StringLength] = '\0';
		}
	}

	String(String&& Other)
	{
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
			SmallValue[StringLength] = '\0';
		}
		else
		{
			Value = Funky::Move(Other.Value);
			Other.BufferCapacity = 0u;
			Other.StringLength = 0u;
		}
	}

	void operator=(T const* const InCStr)
	{
		const size PrevLength = StringLength;
		StringLength = StrLen(InCStr);

		if (StringLength < SMALL_STRING)
		{
			MemCpy(InCStr, SmallValue, StringLength);
			SmallValue[StringLength] = '\0';
		}
		else
		{
			if (StringLength + 1 > BufferCapacity)
				Allocate(StringLength + 1);

			MemCpy(InCStr, Value.Get(), StringLength);
			Value.Get()[StringLength] = '\0';
		}
	}

	void operator=(String const & Other)
	{
		const size PrevLength = StringLength;
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
		}
		else
		{
			if (StringLength + 1 > BufferCapacity)
				Allocate(StringLength + 1);

			MemCpy(Other.Value.Get(), Value.Get(), StringLength);
			Value.Get()[StringLength] = '\0';
		}
	}

	void operator=(String&& Other)
	{
		const size PrevLength = StringLength;
		StringLength = Other.StringLength;

		if (StringLength < SMALL_STRING)
		{
			MemCpy(Other.SmallValue, SmallValue, StringLength);
		}
		else
		{
			Value = Funky::Move(Other.Value);
			Other.BufferCapacity = 0u;
			Other.StringLength = 0u;
		}
	}

	bool operator==(String const & Other) const
	{
		if (Other.StringLength != StringLength) 
			return false;

		auto OtherBuffer = Other.GetBuffer();
		auto ThisBuffer = GetBuffer();

		for (size i = 0u; i < StringLength; ++i)
		{
			if (OtherBuffer[i] != ThisBuffer[i])
				return false;
		}

		return true;
	}

	T const* const GetBuffer() const
	{
		if (StringLength < SMALL_STRING)
			return SmallValue;

		return Value.Get();
	}

	size Length() const { return StringLength; }
	size Capacity() const { return BufferCapacity; }
	bool IsEmpty() const { return StringLength == 0u; }

	bool EndsWith(String const & Other) const
	{
		const size OtherLength = Other.StringLength;
		if (OtherLength > StringLength) 
			return false;

		const size Start = StringLength - OtherLength;
		auto MyBuffer = GetBuffer();
		auto OtherBuffer = Other.GetBuffer();

		for (size i = 0; i < OtherLength; ++i)
		{
			if (MyBuffer[Start + i] != OtherBuffer[i])
				return false;
		}

		return true;
	}
	bool StartsWith(String const& Other)
	{
		const size OtherLength = Other.StringLength;
		if (OtherLength > StringLength)
			return false;

		auto MyBuffer = GetBuffer();
		auto OtherBuffer = Other.GetBuffer();

		for (size i = 0; i < OtherLength; ++i)
		{
			if (MyBuffer[i] != OtherBuffer[i])
				return false;
		}

		return true;
	}

private:
	void Allocate(size NewCapacity)
	{
		BufferCapacity = NewCapacity;
		Value.Reset(new T[BufferCapacity]);
	}


	size StringLength = 0u;

	T SmallValue[SMALL_STRING];

	size BufferCapacity = 0u;
	Funky::Core::Memory::UniquePtr<T[]> Value;
};

using Str = String<char>;