#include "Camera.h"



void Math::Camera::RecalculateView() const
{
	Math::Vec3f NewForward(0.0f, 0.0f, 100.f);

	NewForward = NewForward.RotateX(Rotation.X);
	NewForward = NewForward.RotateY(Rotation.Y);
	NewForward = NewForward.RotateZ(Rotation.Z);

	Math::Vec3f LookAt = Position + NewForward;

	View = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(Position.X, Position.Y, Position.Z, 0.0f),
		DirectX::XMVectorSet(LookAt.X, LookAt.Y, LookAt.Z, 0.0f),
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
//{
//	DirectX::XMMATRIX NewView = DirectX::XMMatrixIdentity();
//	
//	NewView *= DirectX::XMMatrixRotationRollPitchYaw(
//		Math::ToRad(Rotation.X),
//		Math::ToRad(Rotation.Y),
//		Math::ToRad(Rotation.Z)
//	);
//
//	//NewView *= DirectX::XMMatrixTranslation(
//	//	-(Right.Dot(Position)),
//	//	-(Up.Dot(Position)), 
//	//	-(Forward.Dot(Position))
//	//);
//
//
//	
//	
//	//NewView *= DirectX::XMMatrixTranslation(-Position.X, -Position.Y, -Position.Z);
//
//
	 
//
//	// global transform
//
//	// relative transform
//
//	//NewView.r[3] = DirectX::XMVectorSet( 1.0f);
//	View = NewView;
//
//	bViewDirty = false;
//}
