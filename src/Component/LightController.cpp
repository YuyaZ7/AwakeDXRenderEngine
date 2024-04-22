#include "LightController.h"

LightController::LightController(uint maxDirect, uint maxPoint, uint maxSpot) : info(),
																				maxDirectLight(maxDirect),
																				maxPointLight(maxPoint),
																				maxSpotLight(maxSpot),
																				lightData(new vbyte[sizeof(info) + maxDirect * DirectionalLight::GetRawSize() + maxPoint * PointLight::GetRawSize() + maxSpot * SpotLight::GetRawSize()])

{
	directLights.reserve(maxDirect);
	pointLights.reserve(maxPoint);
	spotLights.reserve(maxSpot);
	*reinterpret_cast<LightInfo*>(lightData.get()) = info;
}

void LightController::SetEyePosition(XMFLOAT3 position) {
	info.eyePos = position;
	reinterpret_cast<LightInfo*>(lightData.get())->eyePos = position;
}

bool LightController::AddLight(DirectionalLight* directLight) {
	if (directLights.size() >= maxDirectLight) {
		printf("Exceed direct light limits/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += directLights.size() * DirectionalLight::GetRawSize() + sizeof(info);
	*reinterpret_cast<DirectionalLight::RawData*>(ptr) = directLight->GetRawData();
	directLights.push_back(directLight);
	reinterpret_cast<LightInfo*>(lightData.get())->directNum++;
	return true;
}

bool LightController::AddLight(PointLight* pointLight) {
	if (pointLights.size() >= maxPointLight) {
		printf("Exceed direct light limits/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += pointLights.size() * PointLight::GetRawSize() + maxDirectLight * DirectionalLight::GetRawSize() + sizeof(info);
	*reinterpret_cast<PointLight::RawData*>(ptr) = pointLight->GetRawData();
	pointLights.push_back(pointLight);
	reinterpret_cast<LightInfo*>(lightData.get())->pointNum++;
	return true;
}

bool LightController::AddLight(SpotLight* spotLight) {
	if (spotLights.size() >= maxSpotLight) {
		printf("Exceed direct light limits/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += spotLights.size() * SpotLight::GetRawSize() + maxPointLight * PointLight::GetRawSize() + maxDirectLight * DirectionalLight::GetRawSize() + sizeof(info);
	*reinterpret_cast<SpotLight::RawData*>(ptr) = spotLight->GetRawData();
	spotLights.push_back(spotLight);
	reinterpret_cast<LightInfo*>(lightData.get())->spotNum++;
	return true;
}

bool LightController::UpdateLight(DirectionalLight* directLight, uint index) {
	if (index >= directLights.size()) {
		printf("Index out of range/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += index * DirectionalLight::GetRawSize() + sizeof(info);
	*reinterpret_cast<DirectionalLight::RawData*>(ptr) = directLight->GetRawData();
	return true;
}

const vbyte* LightController::GetLightConstantBuffer() {
	return lightData.get();
}

size_t LightController::GetSize() {
	return sizeof(info) + maxDirectLight * DirectionalLight::GetRawSize() + maxPointLight * sizeof(PointLight);
}