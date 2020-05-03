/** @file MathGlobals.h
* All mathematics constants and functions wrapped into Funky namespace constexpr function helpers;
*/

#pragma once

#include <cmath>

namespace Math
{
	template<typename T>
	constexpr T PI = T(3.14159265358979323846264338327950288419716939937510);

	template <typename T>
	constexpr T ToDeg(T Theta) { return (Theta * T(180.0) / PI<T>); }

	template <typename T>
	constexpr T ToRad(T Theta) { return (Theta * PI<T> / T(180.0)); }

	template <typename T>
	constexpr T Abs(T InValue)
	{
		return abs(InValue);
	}

	template <typename T>
	constexpr T Cos(T InValue)
	{
		return cos(ToRad(InValue));
	}

	template <typename T>
	constexpr T Sin(T InValue)
	{
		return sin(ToRad(InValue));
	}

	template <typename T>
	constexpr T Atan2(T InX, T InY)
	{
		return atan2(InY, InX);
	}

	template <typename T>
	constexpr T Sqrt(T InValue)
	{
		return sqrt(InValue);
	}

	/** @InValue in degrees. */
	template <typename T>
	constexpr T Tan(T InValue)
	{
		return tan(ToRad(InValue));
	}

	template <typename T>
	constexpr T Max(T A, T B)
	{
		return A > B ? A : B;
	}

	template <typename T>
	constexpr T Min(T A, T B)
	{
		return A < B ? A : B;
	}

}
