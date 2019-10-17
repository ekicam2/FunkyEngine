#pragma once

#include <vector>
#include <unordered_map>

// dynamic array
template <typename T>
using  darray = std::vector<T>;

// static array
template <typename T, int Size>
using sarray = std::array<T, Size>;

template <typename K, typename V>
using map = std::unordered_map<K, V>;