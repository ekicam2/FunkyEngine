#pragma once

#include <string>
typedef std::string str;


#include "Memory/Allocator.h"

namespace Funky
{
	template <typename T, typename Allocator = Funky::Allocator<T>>
	class String
	{

	};
}