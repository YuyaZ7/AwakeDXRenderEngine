#include "LightController.h"

LightController::LightController(uint maxDirect, uint maxPoint) : info(),
																  maxDirectLight(maxDirect),
																  maxPointLight(maxPoint),
																  lightData(new vbyte[sizeof(info) + maxDirect * sizeof(DirectLight) + maxPoint * sizeof(PointLight)]) {
	directLights.reserve(maxDirect);
	pointLights.reserve(maxPoint);
	*reinterpret_cast<LightInfo*>(lightData.get()) = info;
}
bool LightController::AddLight(DirectLight directLight) {
	if (directLights.size() >= maxDirectLight) {
		printf("Exceed direct light limits/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += directLights.size() * sizeof(DirectLight) + sizeof(info);
	*reinterpret_cast<DirectLight*>(ptr) = directLight;
	directLights.push_back(directLight);
	reinterpret_cast<LightInfo*>(lightData.get())->directNum++;
	return true;
}
void LightController::SetEyePosition(XMFLOAT3 position) {
	info.eyePos = position;
	reinterpret_cast<LightInfo*>(lightData.get())->eyePos = position;
}
bool LightController::AddLight(PointLight pointLight) {
	if (pointLights.size() >= maxPointLight) {
		printf("Exceed direct light limits/n");
		return false;
	}
	vbyte* ptr = lightData.get();
	ptr += pointLights.size() * sizeof(PointLight) + maxDirectLight * sizeof(DirectLight) + sizeof(info);
	*reinterpret_cast<PointLight*>(ptr) = pointLight;
	pointLights.push_back(pointLight);
	reinterpret_cast<LightInfo*>(lightData.get())->pointNum++;
	return true;
}
const vbyte* LightController::GetLightConstantBuffer() {
	return lightData.get();
}
size_t LightController::GetSize() {
	return sizeof(info) + maxDirectLight * sizeof(DirectLight) + maxPointLight * sizeof(PointLight);
}