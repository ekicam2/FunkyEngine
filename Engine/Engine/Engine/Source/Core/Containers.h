#pragma once

#include <vector>

// dynamic array
template <typename T>
using  darray = std::vector<T>;

// static array
template <typename T, int Size>
using sarray = std::array<T, Size>;