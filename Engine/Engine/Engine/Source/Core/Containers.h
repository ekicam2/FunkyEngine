#pragma once

#include "BasicTypes.h"

#include <vector>
#include <unordered_map>
#include <algorithm>


// dynamic array
template <typename T>
using  darray = std::vector<T>;

// static array
template <typename T, i32 Size>
using sarray = std::array<T, Size>;

template <typename K, typename V>
using map = std::unordered_map<K, V>;