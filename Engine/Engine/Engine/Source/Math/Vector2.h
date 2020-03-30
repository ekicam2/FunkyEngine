#pragma once

#include "BasicTypes.h"

namespace Math
{
	/** Two dimensional template Vector. */
	template <typename T>
	struct Vector2D
	{
		T X, Y;

		Vector2D(T X, T Y) : X(X), Y(Y) { }

		Vector2D operator+(Vector2D const & Rhs)
		{
			return { X + Rhs.X, Y + Rhs.Y};
		}

		void operator+=(Vector2D const & Rhs)
		{
			X += Rhs.X;
			Y += Rhs.Y;
		}

		Vector2D operator-(Vector2D const & Rhs)
		{
			return { X - Rhs.X, Y - Rhs.Y };
		}

		Vector2D operator*(T Scalar)
		{
			return { X * Scalar, Y * Scalar };
		}

		Vector2D& operator=(Vector2D const & New)
		{
			X = New.X;
			Y = New.Y;

			return *this;
		}

		bool operator==(Vector2D const& Other) const
		{
			return X == Other.X && Y == Other.Y;
		}

		bool operator!=(Vector2D const& Other) const
		{
			return !operator==(Other);
		}

		Vector2D operator/(T Scalar)
		{
			return { X / Scalar, Y / Scalar };
		}


		T Dot(Vector2D const & Rhs)
		{
			return  X * Rhs.X + Y * Rhs.Y;
		}

		T Length()
		{
			return Sqrt(X * X + Y * Y);
		}
	};

	using Vec2f = Vector2D<f32>; //!< Template instance for 2 dimensional float vector 
	using Vec2d = Vector2D<f64>; //!< Template instance for 2 dimensional double vector 
	using Vec2u = Vector2D<u32>; //!< Template instance for 2 dimensional unsigned vector 
}

// standards says so byt anyway it was great exercise for macros related stuff
//#define _FY_TEST_VECTOR2_INLINE_ALLOC(in_type, str_type)						\
//	static_assert(																\
//		   offsetof(Math::Vector2D<in_type>, X) == 0 * sizeof(in_type) 	\
//		&& offsetof(Math::Vector2D<in_type>, Y) == 1 * sizeof(in_type) 	\
//		, "Math::Vector2D<"##in_type##"> X Y are not placed continously");
//
//#define FY_TEST_VECTOR2_INLINE_ALLOC(in_type) _FY_TEST_VECTOR3_INLINE_ALLOC(in_type, #in_type)
//
//FY_TEST_VECTOR2_INLINE_ALLOC(f32);
//FY_TEST_VECTOR2_INLINE_ALLOC(f64);
//FY_TEST_VECTOR2_INLINE_ALLOC(u32);