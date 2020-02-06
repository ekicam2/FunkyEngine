#pragma once

#include "BasicTypes.h"
#include "Math/Vector3.h"

namespace Math
{
	template <typename T>
	struct Vector4D
	{
		T X, Y, Z, W;

		Vector4D(T X, T Y, T Z, T W)
			: X(X)
			, Y(Y)
			, Z(Z)
			, W(W)
		{
		}

		Vector4D(Vector3D<T> const& XYZ, T W)
			: X(XYZ.X)
			, Y(XYZ.Y)
			, Z(XYZ.Z)
			, W(W)
		{
		}

		Vector3D<T> AsVec3() const
		{
			return Vector3D<T>{ X, Y, Z };
		}

		Vector4D operator-(Vector4D const & Rhs) const
		{
			return { X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z, W - Rhs.W };
		}

		Vector4D Cross(Vector4D const & Rhs) const
		{
			return Vector4D(AsVec3().Cross(Rhs.AsVec3()), 0.0);
		}
	};



	using Vec4f = Vector4D<f32>;
	static_assert(sizeof(Vec4f) == 16, "Vec4f was not 16 bytes large");

}