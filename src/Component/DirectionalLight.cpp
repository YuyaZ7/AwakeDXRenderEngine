#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const XMFLOAT3& intensity, const XMFLOAT3& position, const XMFLOAT3& right,
								   const XMFLOAT3& forward, const XMFLOAT3& up, uint shadowResolution, Device* device, bool isCastShadow)
	: intensity(intensity), position(position), forward(forward), right(right), up(up), shadowResolution(shadowResolution), isCastShadow(isCastShadow) {
	if (isCastShadow) {
		shadowMap =
			new ShadowMap(device, shadowResolution, shadowResolution, DXGI_FORMAT_D32_FLOAT, 1, D3D12_RESOURCE_STATE_GENERIC_READ);
		UpdateLightViewMatrix();
		UpdateLightProjMatrix();
	}
}

void DirectionalLight::SetDirection(XMFLOAT3 newDir) {
	forward = newDir;
	UpdateLightViewMatrix();
};

void DirectionalLight::SetPosition(XMFLOAT3 newPos) {
	position = newPos;
	UpdateLightViewMatrix();
};

XMFLOAT3 DirectionalLight::GetPosition() const {
	return position;
};
XMFLOAT3 DirectionalLight::GetDirection() const {
	return forward;
};
Math::Matrix4 DirectionalLight::GetViewMatrix() const {
	return lightViewMatrix;
}

Math::Matrix4 DirectionalLight::GetProjectionMatrix() const {
	return lightProjMatrix;
}

Math::Matrix4 DirectionalLight::GetViewProjectionMatrix() const {
	return XMMatrixTranspose(this->lightViewMatrix * this->lightProjMatrix);
}

void DirectionalLight::UpdateLightProjMatrix() {
	lightProjMatrix = XMMatrixOrthographicLH(orthoWidth, orthoHeight, zn, zf);
}
void DirectionalLight::UpdateLightViewMatrix() {
	XMVECTOR P = XMLoadFloat3(&position);
	XMVECTOR L = XMLoadFloat3(&forward);
	XMVECTOR R, U;

	L = XMVector3Normalize(L);
	R = XMVector3Cross(XMLoadFloat3(&up), L);
	R = XMVector3Normalize(R);
	U = XMVector3Cross(L, R);

	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
	XMStoreFloat3(&forward, L);

	lightViewMatrix = XMMatrixLookAtLH(P, P + L, U);
}

ShadowMap* DirectionalLight::GetShadowMap() const {
	return shadowMap;
}

DirectionalLight::RawData DirectionalLight::GetRawData() {
	return RawData{.Strength = this->intensity,
				   .isCastShadow = this->isCastShadow,
				   .Direction = this->forward,
				   .padding0 = 1,
				   .lightViewProjMatrix = GetViewProjectionMatrix()};
}

void DirectionalLight::CreateView4ShadowMap(DescriptorHeap* dsvHeap, DescriptorHeap* srvHeap, uint miplevels) {
	if (isCastShadow) {
		shadowMap->CreateSRV(srvHeap, miplevels);
		shadowMap->CreateDSV(dsvHeap, miplevels);
	} else {
		exit(0);
	}
}
