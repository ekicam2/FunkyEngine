#pragma once

#include "BasicTypes.h"

#include "DebugMacros.h"

#include <Math/Vector4.h>
#include <Math/Vector3.h>

#include "LogMacros.h"

namespace Math
{
	template<typename T>
	struct Matrix4
	{
		using MyVec = Vector4D<T>;
		using MyVec3 = Vector3D<T>;

		const static Matrix4<T> Identity;

		T m[4][4];

		Matrix4() 
		{
			m[0][0] = 0.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
			m[1][0] = 0.0; m[1][1] = 0.0; m[1][2] = 0.0; m[1][3] = 0.0;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 0.0; m[2][3] = 0.0;
			m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 0.0;
		}

		Matrix4(MyVec const & Row1, MyVec const & Row2, MyVec const & Row3, MyVec const & Row4)
		{
			m[0][0] = Row1.X; m[0][1] = Row1.Y; m[0][2] = Row1.Z; m[0][3] = Row1.W;
			m[1][0] = Row2.X; m[1][1] = Row2.Y; m[1][2] = Row2.Z; m[1][3] = Row2.W;
			m[2][0] = Row3.X; m[2][1] = Row3.Y; m[2][2] = Row3.Z; m[2][3] = Row3.W;
			m[3][0] = Row4.X; m[3][1] = Row4.Y; m[3][2] = Row4.Z; m[3][3] = Row4.W;
		}

		Matrix4 operator*(Matrix4 const& Rhs) const 
		{
			Matrix4 Ret;

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

		static void Translate(Matrix4& Modified, MyVec3 const & Offset)
		{
			Modified.m[3][0] += Offset.X; Modified.m[3][1] += Offset.Y; Modified.m[3][2] += Offset.Z;
		}

		static void Scale(Matrix4& Modified, MyVec3 const& Scale)
		{
			Modified.m[0][0] += Math::Sin(Scale.X); Modified.m[1][1] += Math::Sin(Scale.Y); Modified.m[2][2] += Math::Sin(Scale.Z); Modified.m[3][3] += 1.0f;
		}

		static void RotateX(Matrix4& Modified, T const& Rotation)
		{
			Modified.m[1][1] = Cos(Rotation);  Modified.m[1][2] = -Sin(Rotation);
			Modified.m[2][1] = Sin(Rotation);  Modified.m[2][2] = Cos(Rotation);
		}

		static void RotateY(Matrix4& Modified, T const& Rotation)
		{
			Modified.m[0][0] = Cos(Rotation);  Modified.m[0][2]  = Sin(Rotation);
			Modified.m[2][0] = -Sin(Rotation);  Modified.m[2][2] = Cos(Rotation);
		}

		static void RotateZ(Matrix4& Modified, T const& Rotation)
		{
			Modified.m[0][0] = Cos(Rotation);  Modified.m[0][1] = -Sin(Rotation);
			Modified.m[1][0] = Sin(Rotation);  Modified.m[1][1] = Cos(Rotation);
		}

		static void Rotate(Matrix4& Modified, MyVec3 const& Rotator)
		{
			Matrix4::RotateX(Modified, Rotator.X);
			Matrix4::RotateY(Modified, Rotator.Y);
			Matrix4::RotateZ(Modified, Rotator.Z);
		}

		static void Transpose(Matrix4& Modified)
		{
			Matrix4 Copy = Modified;

			Modified.m[0][0] = Copy.m[0][0]; Modified.m[0][1] = Copy.m[1][0]; Modified.m[0][2] = Copy.m[2][0]; Modified.m[0][3] = Copy.m[3][0];
			Modified.m[1][0] = Copy.m[0][1]; Modified.m[1][1] = Copy.m[1][1]; Modified.m[1][2] = Copy.m[2][1]; Modified.m[1][3] = Copy.m[3][1];
			Modified.m[2][0] = Copy.m[0][2]; Modified.m[2][1] = Copy.m[1][2]; Modified.m[2][2] = Copy.m[2][2]; Modified.m[2][3] = Copy.m[3][2];
			Modified.m[3][0] = Copy.m[0][3]; Modified.m[3][1] = Copy.m[1][3]; Modified.m[3][2] = Copy.m[2][3]; Modified.m[3][3] = Copy.m[3][3];
		}


		static Matrix4 LookAtLH(MyVec3 const& Position, MyVec3 const& Destination, MyVec3 const& Up)
		{
			CHECK(Up.Length() == 1u);

			const MyVec3 NewForward = (Destination - Position).Normalized();
			const MyVec3 NewRight	= Up.Cross(NewForward).Normalized();
			const MyVec3 NewUp		= NewForward.Cross(NewRight).Normalized();
			
			const MyVec3 ABC(
				-(NewRight.Dot(Position)),
				-(NewUp.Dot(Position)),
				-(NewForward.Dot(Position))
			);
			
			Matrix4 Ret = Matrix4(
				MyVec(NewRight, 0.0), 
				MyVec(NewUp, 0.0), 
				MyVec(NewForward, 0.0), 
				MyVec(ABC, 1.0)
			);

			return Ret;
		}

		static Matrix4 ProjectionMatrixLH(T AspectRatio, T VerticalFov, T Near, T Far)
		{
			MyVec Row1(T(1.0) / (AspectRatio * Tan(VerticalFov * T(0.5))), T(0.0), T(0.0), T(0.0));
			MyVec Row2(T(0.0), T(1.0) / (Tan(VerticalFov * T(0.5))), T(0.0), T(0.0));
			MyVec Row3(T(0.0), T(0.0), (Far/(Far-Near)), T(1.0));
			MyVec Row4(T(0.0), T(0.0), -(Far*Near/(Far - Near)) , T(0.0));

			return Matrix4(Row1, Row2, Row3, Row4);
		}
	
	};


	using Mat4f = Matrix4<f32>;
	static_assert(sizeof(Mat4f) == 64, "Mat4f was not 64 bytes large");
}

template<typename T>
const Math::Matrix4<T> Math::Matrix4<T>::Identity = Math::Matrix4<T>({ 1.0, 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0, 1.0 });