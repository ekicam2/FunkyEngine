#pragma once

#include "fk_string.h"

namespace fk
{
	class Path
	{
	public:
		Path(str const & path);
		bool isAbsolute() const;
		str const & operator*() const;

	private:
		str path;
	};
}

#ifdef FK_IMPLEMENTATION
fk::Path::Path(str const & path)
	: path(path)
{
	
}

void fk::Path::isAbsolute() const
{
	return (*this)[1] ==
#if _UNICODE
		L':'
#else
		':'
#endif 
		&&
		(*this)[2] ==
#if _UNICODE
		L':'
#else
		':'
#endif 
		;
}

str const & fk::Path::operator*() const
{
	return path;
}

#endif