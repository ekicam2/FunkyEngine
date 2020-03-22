#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4.h"

#define FORCEINLINE __forceinline

namespace Math
{
	class Camera
	{
	public:
		Camera()
			: Projection(Math::Mat4f::Identity)
			, Position(0.0f, 0.0f, 0.0f)
			, Rotation(0.0f, 0.0f, 0.0f)
			, LookAt(0.0f, 0.0f, 100.f)
			, Up(0.0f, 1.0f, 0.0f)
		{
		}

		Camera(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
			: Camera()
		{
			MakePerepective(AspectRatio, FOV, Near, Far);
		}

		void MakePerepective(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
		{
			Projection = Math::Mat4f::ProjectionMatrixLH(AspectRatio, FOV, Near, Far); 
			RecalculateView();
		}

		void MakeOrtho([[maybe_unused]]float Width, [[maybe_unused]] float Height, [[maybe_unused]] float Near = 0.01f, [[maybe_unused]] float Far = 100.0f)
		{
			//Projection = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
			//RecalculateView();
		}

		FORCEINLINE Math::Mat4f const & GetProjection() const { return Projection; }

		FORCEINLINE Math::Mat4f const & GetView() const
		{
			if (bViewDirty)
				RecalculateView();
			return View;
		}

		FORCEINLINE Math::Vec3f const & GetLookat() const { return LookAt; }

		FORCEINLINE Math::Vec3f const & GetPosition() const { return Position; }

		void Translate(Math::Vec3f const & vTranslation)
		{
			Math::Vec3f Temp = vTranslation;

			Temp = Temp.RotateX(Rotation.X);
			Temp = Temp.RotateY(Rotation.Y);
			Temp = Temp.RotateZ(Rotation.Z);

			Position += Temp;
			bViewDirty = true;
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
			Math::Vec3f Forward(0.0f, 0.0f, 100.f);

			Forward = Forward.RotateX(Rotation.X);
			Forward = Forward.RotateY(Rotation.Y);
			Forward = Forward.RotateZ(Rotation.Z);

			LookAt = Position + Forward;

			View = Math::Mat4f::LookAtLH(Position, LookAt, Up);

			//DirectX::XMMatrixLookAtLH(
			//	DirectX::XMMatrixLookAtLH(
			//	DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
			//	DirectX::XMVectorSet(LookAt.X, LookAt.Y, LookAt.Z, 0.0f),
			//	DirectX::XMVectorSet(Up.X, Up.Y, Up.Z, 0.0f)
			//);

			bViewDirty = false;
		}

		mutable bool bViewDirty = false;

		Math::Vec3f Position;
		Math::Vec3f Rotation;
		mutable Math::Vec3f LookAt;
		const Math::Vec3f Up;

		Math::Mat4f Projection;

		mutable Math::Mat4f View = Math::Mat4f::LookAtLH(Position, LookAt, Up); 
		//DirectX::XMMatrixLookAtLH(
		//	DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
		//	DirectX::XMVectorSet(LookAt.X, LookAt.Y, LookAt.Z, 0.0f),
		//	DirectX::XMVectorSet(Up.X, Up.Y, Up.Z, 0.0f)
		//);
	};
}