#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4.h"

#include <DirectXMath.h>


#define FORCEINLINE __forceinline

namespace Math
{
	class Camera
	{
	public:
		Camera()
			: Position(0.0f, 0.0f, 0.0f)
			, Rotation(0.0f, 0.0f, 0.0f)
			, Up(0.0f, 1.0f, 0.0f)
			, Right(1.0f, 0.0f, 0.0f)
			, Forward(0.0f, 0.0f, 1.0f)
			, Projection(DirectX::XMMatrixIdentity())
			, View(DirectX::XMMatrixIdentity())
		{
		}

		Camera(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
			: Camera()
		{
			MakePerspective(AspectRatio, FOV, Near, Far);
		}

		void MakePerspective(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
		{
			Projection = DirectX::XMMatrixPerspectiveFovLH(Math::ToRad(FOV), AspectRatio, Near, Far);
		}

		void MakeOrtho([[maybe_unused]]float Width, [[maybe_unused]] float Height, [[maybe_unused]] float Near = 0.01f, [[maybe_unused]] float Far = 100.0f)
		{
			//Projection = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
			//RecalculateView();
		}

		FORCEINLINE DirectX::XMMATRIX  const & GetProjection() const { return Projection; }

		FORCEINLINE DirectX::XMMATRIX  const & GetView() const
		{
			if (bViewDirty)
				RecalculateView();
			return View;
		}

		FORCEINLINE Math::Vec3f const& GetForward() const { return Forward; }
		FORCEINLINE Math::Vec3f const& GetRight() const { return Right; }

		FORCEINLINE Math::Vec3f const& GetPosition() const { return Position; }

		FORCEINLINE Math::Vec3f const& GetRotation() const { return Rotation; }

		void Translate(Math::Vec3f const & vTranslation)
		{
			//Math::Vec3f Temp = vTranslation;
			//Temp = Temp.RotateX(Rotation.X);
			//Temp = Temp.RotateY(Rotation.Y);
			//Temp = Temp.RotateZ(Rotation.Z);

			Position += vTranslation;
			bViewDirty = true;
		}

		void SetPosition(Math::Vec3f const& vPosition, bool bRotate = false)
		{
			Math::Vec3f Temp = vPosition;
			if (bRotate)
			{
				Temp = Temp.RotateX(Rotation.X);
				Temp = Temp.RotateY(Rotation.Y);
				Temp = Temp.RotateZ(Rotation.Z);
			}

			Position = vPosition;
			bViewDirty = true;
		}

		void SetRotation(Math::Vec3f const& vRotation)
		{
			Rotation = vRotation;
			bViewDirty = true;
		}

		/**
		* Rotates camera to look at given point.
		* Sp GetForward() would be (vPosition - vCameraPosition).Normalized().
		* @param vPosition position that camera should be looking at.
		*/
		void LookAt(Math::Vec3f vPosition)
		{
			View = DirectX::XMMatrixLookAtLH(
				DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
				DirectX::XMVectorSet(vPosition.X, vPosition.Y, vPosition.Z, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			);

			DirectX::XMVECTOR const& row1 = View.r[0];
			DirectX::XMVECTOR const& row2 = View.r[1];
			DirectX::XMVECTOR const& row3 = View.r[2];

			Right.X = DirectX::XMVectorGetX(row1);
			Right.Y = DirectX::XMVectorGetX(row2);
			Right.Z = DirectX::XMVectorGetX(row3);

			Up.X = DirectX::XMVectorGetY(row1);
			Up.Y = DirectX::XMVectorGetY(row2);
			Up.Z = DirectX::XMVectorGetY(row3);

			Forward.X = DirectX::XMVectorGetZ(row1);
			Forward.Y = DirectX::XMVectorGetZ(row2);
			Forward.Z = DirectX::XMVectorGetZ(row3);

			bViewDirty = false;
		}

		/**
		* Rotates camera and clamp final rotation.
		* @param vRotation an Euler rotation vector
		*/
		void Rotate(Math::Vec3f const & vRotation) { Rotation += vRotation; bViewDirty = true; }

		/**
		* Rotates camera and clamp final rotation.
		* @param vRotation an Euler rotation vector
		* @param vAxisToClamp describes which axis should be clampped, simply just pass 1.0f on selected axis eg. {0.0f, 1.0f, 0.0f} or {0.0f, 1.0f, 1.0f}
		* @param vAxisMax describes maximum value for axis
		*/
		void RotateClamped(Math::Vec3f const & vRotation, Math::Vec3f const & vAxisToClamp, Math::Vec3f const & vAxisMax)
		{
			Rotation += vRotation;

			if (1.0f == vAxisToClamp.X && Math::Abs(Rotation.X) > vAxisMax.X)
				Rotation.X = vAxisMax.X * (Rotation.X / Math::Abs(Rotation.X));

			if (1.0f == vAxisToClamp.Y && Math::Abs(Rotation.Y) > vAxisMax.Y)
				Rotation.Y = vAxisMax.Y * (Rotation.Y / Math::Abs(Rotation.Y));

			if (1.0f == vAxisToClamp.Z && Math::Abs(Rotation.Z) > vAxisMax.Z)
				Rotation.Z = vAxisMax.Z * (Rotation.Z / Math::Abs(Rotation.Z));

			bViewDirty = true;
		}

	private:
		void RecalculateView() const
		{
			Math::Vec3f NewForward(0.0f, 0.0f, 100.f);

			NewForward = NewForward.RotateX(Rotation.X);
			NewForward = NewForward.RotateY(Rotation.Y);
			NewForward = NewForward.RotateZ(Rotation.Z);

			Math::Vec3f LookAtPosition = Position + NewForward;

			View = DirectX::XMMatrixLookAtLH(
				DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
				DirectX::XMVectorSet(LookAtPosition.X, LookAtPosition.Y, LookAtPosition.Z, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			);

			DirectX::XMVECTOR const& row1 = View.r[0];
			DirectX::XMVECTOR const& row2 = View.r[1];
			DirectX::XMVECTOR const& row3 = View.r[2];

			Right.X = DirectX::XMVectorGetX(row1);
			Right.Y = DirectX::XMVectorGetX(row2);
			Right.Z = DirectX::XMVectorGetX(row3);

			Up.X = DirectX::XMVectorGetY(row1);
			Up.Y = DirectX::XMVectorGetY(row2);
			Up.Z = DirectX::XMVectorGetY(row3);

			Forward.X = DirectX::XMVectorGetZ(row1);
			Forward.Y = DirectX::XMVectorGetZ(row2);
			Forward.Z = DirectX::XMVectorGetZ(row3);

			bViewDirty = false;
		}

		mutable bool bViewDirty = false;

		Math::Vec3f Position;
		Math::Vec3f Rotation;

		mutable Math::Vec3f Up;
		mutable Math::Vec3f Right;
		mutable Math::Vec3f Forward;

		DirectX::XMMATRIX Projection;

		//mutable Math::Mat4f View = mutable Math::Mat4f View = Math::Mat4f::LookAtLH(Position, LookAt, Up);
		/*
		 *		View Matrix layout
		 *
		 * | Right.X | Up.X | Forward.X | 0.0 |
		 * | Right.Y | Up.Y | Forward.Y | 0.0 |
		 * | Right.Z | Up.Z | Forward.Z | 0.0 |
		 * |    A    |  B   |     C     | 1.0 |
		 */
		mutable DirectX::XMMATRIX View;
	};
}