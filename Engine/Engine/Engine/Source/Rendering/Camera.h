#pragma once

#include "Math/Vector3.h"

namespace Funky
{
	class Camera
	{
	public:
		Camera(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
			: Projection(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), AspectRatio, Near, Far))
			, Position(0.0f, 0.0f, 0.0f)
			, Rotation(0.0f, 0.0f, 0.0f)
			, LookAt(0.0f, 0.0f, 100.f)
			, Up(0.0f, 1.0f, 0.0f)
		{
			RecalculateView();
		}

		void MakePerepective(float AspectRatio, float FOV = 90.0f, float Near = 0.01f, float Far = 100.0f)
		{
			Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), AspectRatio, Near, Far);
			RecalculateView();
		}

		void MakeOrtho(float Width, float Height, float Near = 0.01f, float Far = 100.0f)
		{
			Projection = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
			RecalculateView();
		}

		FORCEINLINE DirectX::XMMATRIX const & GetProjection() const { return Projection; }

		FORCEINLINE DirectX::XMMATRIX const & GetView()
		{ 
			if (bViewDirty) 
				RecalculateView(); 
			return m_mView; 
		}
	
		FORCEINLINE Funky::Math::Vector3f const & GetLookat() { return LookAt; }

		FORCEINLINE Funky::Math::Vector3f const & GetPosition() { return Position; }
		
		void Translate(Funky::Math::Vector3f const & vTranslation) 
		{ 
			Funky::Math::Vector3f Temp = vTranslation;

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
		void Rotate(Funky::Math::Vector3f const & vRotation) { Rotation += vRotation; bViewDirty = true; }

		/**
		* Rotates camera and clamp final rotation.
		* @param vRotation an Euler rotation vector 
		* @param vAxisToClamp describes which axis should be clampped, simply just pass 1.0f on selected axis eg. {0.0f, 1.0f, 0.0f} or {0.0f, 1.0f, 1.0f}
		* @param vAxisMax describes maximum value for axis
		*/
		void RotateClamped(Funky::Math::Vector3f const & vRotation, Funky::Math::Vector3f const & vAxisToClamp, Funky::Math::Vector3f const & vAxisMax)
		{ 
			Rotation += vRotation; 
			
			if (1.0f == vAxisToClamp.X && Funky::Math::Abs(Rotation.X) > vAxisMax.X)
				Rotation.X = vAxisMax.X * (Rotation.X / Funky::Math::Abs(Rotation.X));

			if (1.0f == vAxisToClamp.Y && Funky::Math::Abs(Rotation.Y) > vAxisMax.Y)
				Rotation.Y = vAxisMax.Y * (Rotation.Y / Funky::Math::Abs(Rotation.Y));

			if (1.0f == vAxisToClamp.Z && Funky::Math::Abs(Rotation.Z) > vAxisMax.Z)
				Rotation.Z = vAxisMax.Z * (Rotation.Z / Funky::Math::Abs(Rotation.Z));

			bViewDirty = true; 
		}

	private:
		void RecalculateView()
		{
			Funky::Math::Vector3f Forward(0.0f, 0.0f, 100.f);

			Forward = Forward.RotateX(Rotation.X);
			Forward = Forward.RotateY(Rotation.Y);
			Forward = Forward.RotateZ(Rotation.Z);

			LookAt = Position + Forward;

			m_mView = DirectX::XMMatrixLookAtLH(
				DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
				DirectX::XMVectorSet(LookAt.X, LookAt.Y, LookAt.Z, 0.0f),
				DirectX::XMVectorSet(Up.X, Up.Y, Up.Z, 0.0f)
			);

			bViewDirty = false;
		}

		bool bViewDirty = false;

		Funky::Math::Vector3f Position;
		Funky::Math::Vector3f Rotation;
		Funky::Math::Vector3f LookAt;
		const Funky::Math::Vector3f Up;

		DirectX::XMMATRIX Projection;
	
		DirectX::XMMATRIX m_mView = DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
			DirectX::XMVectorSet(LookAt.X, LookAt.Y, LookAt.Z, 0.0f),
			DirectX::XMVectorSet(Up.X, Up.Y, Up.Z, 0.0f)
		);
	};
}

