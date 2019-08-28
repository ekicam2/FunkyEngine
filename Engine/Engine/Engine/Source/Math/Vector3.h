#pragma once

#include "BasicTypes.h"
#include "MathGlobals.h"

namespace Funky
{
	namespace Math
	{
		template <typename T>
		struct Vector3D
		{
			T X, Y, Z;

			Vector3D(T const & X, T const & Y, T const & Z) : X(X), Y(Y), Z(Z) { }

			Vector3D operator+(Vector3D const & Rhs) const
			{
				return { X + Rhs.X, Y + Rhs.Y, Z + Rhs.Z };
			}

			void operator+=(Vector3D const & Rhs)
			{
				X += Rhs.X;
				Y += Rhs.Y;
				Z += Rhs.Z;
			}

			Vector3D operator-(Vector3D const & Rhs) const
			{
				return { X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z };
			}

			Vector3D operator*(T Scalar) const
			{
				return { X * Scalar, Y * Scalar, Z * Scalar };
			}

			Vector3D& operator=(Vector3D const & New)
			{
				X = New.X;
				Y = New.Y;
				Z = New.Z;

				return *this;
			}

			Vector3D& operator=(T const New[3])
			{
				X = New[0];
				Y = New[1];
				Z = New[2];

				return *this;
			}

			Vector3D RotateX(T Theta) const
			{
				const float CosTheta = Cos(ToRad(Theta));
				const float SinTheta = Sin(ToRad(Theta));

				return {
					X,
					(Y * CosTheta) - (Z * SinTheta),
					(Y * SinTheta) + (Z * CosTheta)
				};
			}

			Vector3D RotateY(T Theta) const
			{
				const float CosTheta = Cos(ToRad(Theta));
				const float SinTheta = Sin(ToRad(Theta));

				return {
					(X * CosTheta) + (Z * SinTheta),
					Y, 
					-(X * SinTheta) + (Z * CosTheta)
				};
			}

			Vector3D RotateZ(T Theta) const
			{
				const float CosTheta = Cos(ToRad(Theta));
				const float SinTheta = Sin(ToRad(Theta));
				
				return {
					(X * CosTheta) - (Y * SinTheta),
					(X * SinTheta) + (Y * CosTheta),
					Z
				};
			}

			T Dot(Vector3D const & Rhs) const
			{
				return  X * Rhs.X + Y * Rhs.Y + Z * Rhs.Z;
			}

			T Cross(Vector3D const & Rhs) const
			{
				return  {
					Y * Rhs.Z - Z * Rhs.Y,
					Z * Rhs.X - X * Rhs.Z,
					X * Rhs.Y - Y * Rhs.X 
				};
			}

			T GetLength() const
			{
				return Sqrt(X * X + Y * Y + Z * Z);
			}

			Vector3D Normalize() const
			{
				const T Length = GetLength();

				return {
					X / Length,
					Y / Length,
					Z / Length
				};
			}
		};

		using Vector3f = Vector3D<f32>; //!< Template instance for 3 dimensional float vector 
		using Vector3d = Vector3D<f64>; //!< Template instance for 3 dimensional double vector
		using Vector3u = Vector3D<u32>; //!< Template instance for 3 dimensional unsigned vector

	}
}


// standards says so byt anyway it was great exercise for macros related stuff
// #define _FY_TEST_VECTOR3_INLINE_ALLOC(in_type, str_type)							\
// 	static_assert(																\
// 		   offsetof(Funky::Math::Vector3D<in_type>, X) == 0 * sizeof(in_type) 	\
// 		&& offsetof(Funky::Math::Vector3D<in_type>, Y) == 1 * sizeof(in_type) 	\
// 		&& offsetof(Funky::Math::Vector3D<in_type>, Z) == 2 * sizeof(in_type)	\
// 		, "Math::Vector3D<"##in_type##"> X Y Z are not placed continously");
// 
// #define FY_TEST_VECTOR3_INLINE_ALLOC(in_type) _FY_TEST_VECTOR3_INLINE_ALLOC(in_type, #in_type)
// 
// FY_TEST_VECTOR3_INLINE_ALLOC(f32);
// FY_TEST_VECTOR3_INLINE_ALLOC(f64);
// FY_TEST_VECTOR3_INLINE_ALLOC(u32);
