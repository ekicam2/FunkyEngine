#pragma once

#include "BasicTypes.h"
#include "MathGlobals.h"

namespace Math
{
	template <typename T>
	struct Vector3D
	{
		T X, Y, Z;

		static const Vector3D Zero;

		Vector3D(T const& X, T const& Y, T const& Z) : X(X), Y(Y), Z(Z) {
		}

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

		void operator*=(T Scalar)
		{
			X *= Scalar;
			Y *= Scalar;
			Z *= Scalar;
		}

		Vector3D operator-(Vector3D const & Rhs) const
		{
			return { X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z };
		}

		Vector3D operator-() const
		{
			return { -X, -Y, -Z };
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

		bool operator==(Vector3D const & Other) const
		{
			return X == Other.X && Y == Other.Y && Z == Other.Z;
		}

		Vector3D RotateX(T Theta) const
		{
			const float CosTheta = Cos(Theta);
			const float SinTheta = Sin(Theta);

			return {
				X,
				(Y * CosTheta) + (Z * SinTheta),
				-(Y * SinTheta) + (Z * CosTheta)
			};
		}

		Vector3D RotateY(T Theta) const
		{
			const float CosTheta = Cos(Theta);
			const float SinTheta = Sin(Theta);

			return {
				(X * CosTheta) - (Z * SinTheta),
				Y, 
				(X * SinTheta) + (Z * CosTheta)
			};
		}

		Vector3D RotateZ(T Theta) const
		{
			const float CosTheta = Cos(Theta);
			const float SinTheta = Sin(Theta);
				
			return {
				(X * CosTheta) + (Y * SinTheta),
				-(X * SinTheta) + (Y * CosTheta),
				Z
			};
		}

		T Dot(Vector3D const & Rhs) const
		{
			return  X * Rhs.X + Y * Rhs.Y + Z * Rhs.Z;
		}

		Vector3D Cross(Vector3D const & Rhs) const
		{
			return  {
				Y * Rhs.Z - Z * Rhs.Y,
				Z * Rhs.X - X * Rhs.Z,
				X * Rhs.Y - Y * Rhs.X 
			};
		}

		T Length() const
		{
			return Sqrt(X * X + Y * Y + Z * Z);
		}

		Vector3D Normalized() const
		{
			const T Len = Length();
			return Vector3D(X / Len, Y / Len, Z / Len);
		}
	};

	template <typename T>
	__declspec(selectany) const Math::Vector3D<T> Math::Vector3D<T>::Zero = Vector3D(T(0), T(0), T(0));

	using Vec3f = Vector3D<f32>; //!< Template instance for 3 dimensional float vector 
	using Vec3d = Vector3D<f64>; //!< Template instance for 3 dimensional double vector
	using Vec3u = Vector3D<u32>; //!< Template instance for 3 dimensional unsigned vector

	static_assert(sizeof(Vec3f) == 12, "Vec3f was not 12 bytes large");
	static_assert(sizeof(Vec3d) == 24, "Vec3f was not 12 bytes large");
	static_assert(sizeof(Vec3u) == 12, "Vec3f was not 12 bytes large");
}



// standards says so byt anyway it was great exercise for macros related stuff
// #define _FY_TEST_VECTOR3_INLINE_ALLOC(in_type, str_type)							\
// 	static_assert(																\
// 		   offsetof(Math::Vector3D<in_type>, X) == 0 * sizeof(in_type) 	\
// 		&& offsetof(Math::Vector3D<in_type>, Y) == 1 * sizeof(in_type) 	\
// 		&& offsetof(Math::Vector3D<in_type>, Z) == 2 * sizeof(in_type)	\
// 		, "Math::Vector3D<"##in_type##"> X Y Z are not placed continously");
// 
// #define FY_TEST_VECTOR3_INLINE_ALLOC(in_type) _FY_TEST_VECTOR3_INLINE_ALLOC(in_type, #in_type)
// 
// FY_TEST_VECTOR3_INLINE_ALLOC(f32);
// FY_TEST_VECTOR3_INLINE_ALLOC(f64);
// FY_TEST_VECTOR3_INLINE_ALLOC(u32);
