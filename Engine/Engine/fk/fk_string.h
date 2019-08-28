#pragma once

#include "fk_type_defs.h"
#include "fk_memory.h"

namespace fk
{
	u64 strLen(charx const * const string);

	// check if strings are the same, if you you are looking for strings lengths comparasion
	// use **u64 fk::strLen(charx const * const string)** instead.
	bool strCmp(charx const * const string1, charx const * const string2);

	template<typename TCHAR>
	class _str
	{
	public:
		_str(_str<TCHAR> const & other);
		_str(_str<TCHAR> && other);
		_str(TCHAR const * const string);
		~_str();

		u64 size() const;
		bool isEmpty() const;

		void clear();
		void clear(u64 newCapacity);

		bool operator==(_str<TCHAR> const & other) const;
		_str<TCHAR>& operator=(_str<TCHAR> const & other);
		_str<TCHAR>& operator=(_str<TCHAR>&& other);
		_str<TCHAR>& operator=(TCHAR const * const string);

		TCHAR operator[](u64 index) const;

	private:
		bool isValidIndex(u64 index) const;

		void alloc(u64 newCapacity);
		void realloc(u64 newCapacity);
		void free();

		struct _StrBuffer
		{
			u64 capacity = 0u;
			TCHAR* data = nullptr;
		} buffer;

		u64 num = 0u;
	};

	typedef _str<charx> str;
}

#ifdef FK_IMPLEMENTATION
#if FK_UNKNOWN
u64 __strLen(charx const * const string)
{
	u64 counter = 0u;

	while (string[counter] != 
#if _UNICODE
		L'\0'
#else
		'\0'
#endif
		)
	{
		++counter;
	}

	return counter;
}
#endif

bool __strCmp(charx const * const string1, charx const * const string2)
{
	const u64 s1l = fk::strLen(string1);
	const u64 s2l = fk::strLen(string2);

	if (s1l == s2l)
	{
		for (u64 i = 0; i < s1l; ++i)
		{
			if (string1[i] != string2[i])
				return false;
		}
	}

	return true;
}

u64 fk::strLen(charx const * const string)
{
#if FK_WINDOWS
	#if _UNICODE
		return ::wcslen(string);
	#else
		return ::strlen(string);
	#endif
#else
	return __strLen(string);
#endif
}

bool fk::strCmp(charx const * const string1, charx const * const string2)
{
	return __strCmp(string1, string2);
}

#endif

template<typename TCHAR>
fk::_str<TCHAR>::_str(_str<TCHAR> const & other)
{
	*this = other;
}

template<typename TCHAR>
fk::_str<TCHAR>::_str(TCHAR const * const string)
{
	*this = string;
}

template<typename TCHAR>
fk::_str<TCHAR>::_str(_str && other)
{
	*this = other;
}

template<typename TCHAR>
fk::_str<TCHAR>::~_str()
{
	free();
}

template<typename TCHAR>
u64 fk::_str<TCHAR>::size() const
{
	return num;
}

template<typename TCHAR>
bool fk::_str<TCHAR>::isEmpty() const
{
	return size() > 0u;
}

template<typename TCHAR>
void fk::_str<TCHAR>::clear()
{
	if (buffer.capacity == 0u) 
		return;

	num = 0u;
	buffer.data[0] = '\0';
}

template<typename TCHAR>
void fk::_str<TCHAR>::clear(u64 newCapacity)
{
	if (newCapacity == 0u)
		return;

	if(newCapacity < buffer.capacity)
		realloc(newCapacity);

	num = 0u;

	if(buffer.capacity > 0u)
		buffer.data[0] = '\0';
}

template<typename TCHAR>
bool fk::_str<TCHAR>::operator==(_str<TCHAR> const & other) const
{
	return fk::strCmp(buffer.data, other.buffer.data);
}

template<typename TCHAR>
fk::_str<TCHAR>& fk::_str<TCHAR>::operator=(_str<TCHAR> const & other)
{
	const u64 required = other.num + 1;

	if (required == 1)
		return *this;

	if (buffer.capacity < required)
	{
		realloc(required);
	}

	num = required - 1;
	fk::memCpy(other.buffer.data, buffer.data, required * sizeof(TCHAR));

	return *this;
}

template<typename TCHAR>
fk::_str<TCHAR>& fk::_str<TCHAR>::operator=(_str<TCHAR>&& other)
{
	buffer.capacity = other.buffer.capacity;
	buffer.data = other.buffer.data;

	other.buffer.capacity = 0u;
	other.buffer.data = nullptr;

	return *this;
}

template<typename TCHAR>
fk::_str<TCHAR>& fk::_str<TCHAR>::operator=(TCHAR const * const string)
{
	const u64 required = fk::strLen(string) + 1;

	if (required == 1)
		return *this;

	if (buffer.capacity < required)
	{
		realloc(required);
	}

	num = required - 1;
	fk::memCpy(string, buffer.data, required * sizeof(TCHAR));

	return *this;
}

template<typename TCHAR>
TCHAR fk::_str<TCHAR>::operator[](u64 index) const
{
	if (isValidIndex(index))
		return buffer.data[index];

	return 
#if _UNICODE
		L' '
#else
		' '
#endif
		;
}

// allocation and deallocation costs a bit
// thats why we are rounding allocation up to nearest bigger power of 2
[[maybe_unused]] static u64 naiveallocation(u64 capacity)
{
	u64 counter = 0u;
	u64 rounded = u64(1) << counter;

	while (capacity > rounded)
	{
		rounded = u64(1) << ++counter;
	}

	return rounded;
}

template<typename TCHAR>
bool fk::_str<TCHAR>::isValidIndex(u64 index) const
{
	return size() > 0u && index >= 0u && index < size();
}

template<typename TCHAR>
void fk::_str<TCHAR>::alloc(u64 newCapacity)
{
	u64 roundedSize = naiveallocation(newCapacity);

	buffer.data = new TCHAR[roundedSize];
	buffer.capacity = roundedSize;
}

template<typename TCHAR>
void fk::_str<TCHAR>::realloc(u64 newCapacity)
{
	free();
	alloc(newCapacity);
}

template<typename TCHAR>
void fk::_str<TCHAR>::free()
{
	delete[] buffer.data;
	buffer.data = nullptr;
	buffer.capacity = 0u;
}
