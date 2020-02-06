#pragma once

#include "BasicTypes.h"

#include "DebugMacros.h"

#include <Math/Vector4.h>
#include <Math/Vector3.h>

#include <sstream>
#include "LogMacros.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace Math
{
	template<typename T>
	struct Matrix4D
	{
		using MyVec = Vector4D<T>;
		using MyVec3 = Vector3D<T>;

		const static Matrix4D<T> Identity;

		T m[4][4];

		Matrix4D() 
		{
			m[0][0] = 0.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
			m[1][0] = 0.0; m[1][1] = 0.0; m[1][2] = 0.0; m[1][3] = 0.0;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 0.0; m[2][3] = 0.0;
			m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 0.0;
		}

		Matrix4D(MyVec const & Row1, MyVec const & Row2, MyVec const & Row3, MyVec const & Row4)
		{
			m[0][0] = Row1.X; m[0][1] = Row1.Y; m[0][2] = Row1.Z; m[0][3] = Row1.W;
			m[1][0] = Row2.X; m[1][1] = Row2.Y; m[1][2] = Row2.Z; m[1][3] = Row2.W;
			m[2][0] = Row3.X; m[2][1] = Row3.Y; m[2][2] = Row3.Z; m[2][3] = Row3.W;
			m[3][0] = Row4.X; m[3][1] = Row4.Y; m[3][2] = Row4.Z; m[3][3] = Row4.W;
		}

		Matrix4D operator*(Matrix4D const& Rhs) const 
		{
			Matrix4D Ret;

			for (u8 r = 0; r < 4; ++r)
			{
				for (u8 c = 0; c < 4; ++c)
				{
					Ret.m[r][c] = m[r][0] * Rhs.m[0][c] +
								  m[r][1] * Rhs.m[1][c] +
								  m[r][2] * Rhs.m[2][c] +
								  m[r][3] * Rhs.m[3][c];
				}
			}

			return Ret;
		}

		static void Translate(Matrix4D& Modified, MyVec3 const & Offset)
		{
			Modified.m[3][0] += Offset.X; Modified.m[3][1] += Offset.Y; Modified.m[3][2] += Offset.Z;
		}

		static void Scale(Matrix4D& Modified, MyVec3 const& Scale)
		{
			Modified.m[0][0] += Math::Sin(Scale.X); Modified.m[1][1] += Math::Sin(Scale.Y); Modified.m[2][2] += Math::Sin(Scale.Z); Modified.m[3][3] += 1.0f;
		}

		static void RotateX(Matrix4D& Modified, T const& Rotation)
		{
			Modified.m[1][1] = Cos(Rotation);  Modified.m[1][2] = -Sin(Rotation);
			Modified.m[2][1] = Sin(Rotation);  Modified.m[2][2] = Cos(Rotation);
		}

		static void RotateY(Matrix4D& Modified, T const& Rotation)
		{
			Modified.m[0][0] = Cos(Rotation);  Modified.m[0][2]  = Sin(Rotation);
			Modified.m[2][0] = -Sin(Rotation);  Modified.m[2][2] = Cos(Rotation);
		}

		static void RotateZ(Matrix4D& Modified, T const& Rotation)
		{
			Modified.m[0][0] = Cos(Rotation);  Modified.m[0][1] = -Sin(Rotation);
			Modified.m[1][0] = Sin(Rotation);  Modified.m[1][1] = Cos(Rotation);
		}

		static void Rotate(Matrix4D& Modified, MyVec3 const& Rotator)
		{
			Matrix4D::RotateX(Modified, Rotator.X);
			Matrix4D::RotateY(Modified, Rotator.Y);
			Matrix4D::RotateZ(Modified, Rotator.Z);
		}


		static Matrix4D LookAt(MyVec3 const& Position, MyVec3 const& Destination, MyVec3 const& Up)
		{
			CHECK(Up.Length() == 1u);

			const MyVec3 NewForward = (Destination - Position).Normalized();
			const MyVec3 NewRight = Up.Cross(NewForward).Normalized();

			return Matrix4D(MyVec(NewRight, 0.0), MyVec(Up, 0.0), MyVec(NewForward, 0.0), MyVec(0.0, 0.0, 0.0, 1.0));

		}

		static Matrix4D ProjectionMatrixLH(T AspectRatio, T VerticalFov, T Near, T Far)
		{
			MyVec Row1(T(1.0) / (AspectRatio * Tan(VerticalFov * T(0.5))), T(0.0), T(0.0), T(0.0));
			MyVec Row2(T(0.0), T(1.0) / (Tan(VerticalFov * T(0.5))), T(0.0), T(0.0));
			MyVec Row3(T(0.0), T(0.0), (Far/(Far-Near)), T(1.0));
			MyVec Row4(T(0.0), T(0.0), -(Far / (Far - Near)) , T(0.0));

			// MyVec Row3(T(0.0), T(0.0), T((-Near - Far) / (Far - Near)), T((2.0 * Far * Near)/(Far - Near)));
			// MyVec Row4(T(0.0), T(0.0), T(1.0), T(0.0));

			return Matrix4D(Row1, Row2, Row3, Row4);
		}
	
	};


	using Mat4f = Matrix4D<f32>;
	static_assert(sizeof(Mat4f) == 64, "Mat4f was not 64 bytes large");
}

template<typename T>
const Math::Matrix4D<T> Math::Matrix4D<T>::Identity = Math::Matrix4D<T>({ 1.0, 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0, 1.0 });