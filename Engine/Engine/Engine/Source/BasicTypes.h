/** @file BasicTypes.h
* Single header file used to introduce all built-in types in a cross platform manner. 
*/
#pragma once
#include "Macros.h"


/** represents basic character type may be wchar_t or char, depending on _UNICODE macro */
#ifdef _UNICODE
using charx = wchar_t; 
#else
/** represents basic character type may be wchar_t or char, depending on _UNICODE macro */
using charx = char;
#endif

using byte = unsigned char;//!< represents byte- exactly 8bits

using i8  = __int8;  //!< represents signed byte integer
using i16 = __int16; //!< represents signed two bytes integer
using i32 = __int32; //!< represents signed four bytes integer (basically just int in most compilers)
using i64 = __int64; //!< represents signed four bytes integer

using  u8 = unsigned __int8;  //!< represents unsigned byte integer
using u16 = unsigned __int16; //!< represents unsigned two bytes integer
using u32 = unsigned __int32; //!< represents unsigned four bytes integer (basically just unsigned in most compilers)
using u64 = unsigned __int64; //!< represents unsigned four bytes integer

using f32 = float;	//!< represents single precision floating point number
using f64 = double;	//!< represents double precision floating point number
					
using size = u64;	//!< represents size type


/** @cond */

#define FUNKY_ENSURE_TYPE_SIZE(in_type, expected_size) \
	static_assert(sizeof(in_type) == expected_size, #in_type " was not " #expected_size " bytes large")

FUNKY_ENSURE_TYPE_SIZE(i8,  1);
FUNKY_ENSURE_TYPE_SIZE(i16, 2);
FUNKY_ENSURE_TYPE_SIZE(i32, 4);
FUNKY_ENSURE_TYPE_SIZE(i64, 8);

FUNKY_ENSURE_TYPE_SIZE( u8, 1);
FUNKY_ENSURE_TYPE_SIZE(u16, 2);
FUNKY_ENSURE_TYPE_SIZE(u32, 4);
FUNKY_ENSURE_TYPE_SIZE(u64, 8);

FUNKY_ENSURE_TYPE_SIZE(f32, 4);
FUNKY_ENSURE_TYPE_SIZE(f64, 8);

FUNKY_ENSURE_TYPE_SIZE(size, 8);

/** @endcond */