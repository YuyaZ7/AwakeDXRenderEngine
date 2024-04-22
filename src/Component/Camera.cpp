#include <Component/Camera.h>
using namespace DirectX;
using namespace Math;
Camera::Camera() {
	SetLens(0.33f * MathHelper::Pi, 0.1f, 3000.0f);
}

Camera::~Camera() {
}

void Camera::SetLens(float fovY, float zn, float zf) {
	// cache properties
	mFovY = fovY;
	mNearZ = zn;
	mFarZ = zf;
	mNearWindowHeight = 2.0f * mNearZ * tan(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tan(0.5f * mFovY);
}

void Camera::SetAspect(float aspect) {
	mAspect = aspect;
}

void Camera::LookAt(const Math::Vector3& pos, const Math::Vector3& target, const Math::Vector3& worldUp) {
	Forward = normalize(target - pos);
	Right = normalize(cross(worldUp, Forward));
	Up = cross(Forward, Right);
	Position = pos;
	mViewDirty = true;
}

void Camera::UpdateProjectionMatrix() {
	if (isOrtho) {
		mFarZ = Max(mFarZ, mNearZ + 0.1f);
		Matrix4 P = XMMatrixOrthographicLH(orthoSize, orthoSize, mNearZ, mFarZ);
		Proj = P;
	} else {
		mNearZ = Max(mNearZ, 0.01f);
		mFarZ = Max(mFarZ, mNearZ + 0.1f);
		Matrix4 P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
		Proj = P;
	}
}

void Camera::UpdateViewMatrix() {
	if (mViewDirty) {
		XMVECTOR R = Right;
		XMVECTOR U = Up;
		XMVECTOR L = Forward;
		XMVECTOR P = Position;

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(Forward);
		U = XMVector3Normalize(XMVector3Cross(Forward, Right));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		Right.SetX(XMVectorGetX(R));
		Right.SetY(XMVectorGetY(R));
		Right.SetZ(XMVectorGetZ(R));
		Up.SetX(XMVectorGetX(U));
		Up.SetY(XMVectorGetY(U));
		Up.SetZ(XMVectorGetZ(U));
		Forward.SetX(XMVectorGetX(L));
		Forward.SetY(XMVectorGetY(L));
		Forward.SetZ(XMVectorGetZ(L));
		View = XMMatrixLookAtLH(Position, Position + Forward, Up);
	}
}

void Camera::Strafe(float d) {

	Position += Right * d;
	mViewDirty = true;
}

void Camera::Walk(float d) {
	// mPosition += d*mLook

	Position += Forward * d;
	mViewDirty = true;
}

void Camera::Pitch(float angle) {
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(Right.m_vec, angle);
	Up.m_vec = XMVector3TransformNormal(Up, R);
	Forward.m_vec = XMVector3TransformNormal(Forward, R);
	mViewDirty = true;
}

void Camera::RotateY(float angle) {
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);
	Right.m_vec = XMVector3TransformNormal(Right, R);
	Up.m_vec = XMVector3TransformNormal(Up, R);
	Forward.m_vec = XMVector3TransformNormal(Forward, R);

	mViewDirty = true;
}
