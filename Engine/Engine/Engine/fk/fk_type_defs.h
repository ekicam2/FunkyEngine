#pragma once

#ifdef _UNICODE
using charx = wchar_t;
#else
using charx = char;
#endif

typedef unsigned char byte;

typedef __int8 i8;
typedef __int16 i16;
typedef __int32 i32;
typedef __int64 i64;

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64; 

typedef float f32;
typedef double f64;

typedef u64 size;
