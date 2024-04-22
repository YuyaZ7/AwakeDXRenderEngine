#pragma once
#include <DXMath/DXMath.h>
#include <DXMath/MathHelper.h>
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Metalib.h"
#include <span>

//Global light info
struct LightInfo {
	LightInfo() : directNum(0), pointNum(0), spotNum(0), padding0(0), eyePos(), padding1(1){};
	uint directNum;
	uint pointNum;
	uint spotNum;
	uint padding0;
	XMFLOAT3 eyePos;
	uint padding1;
};

class LightController final {
	LightInfo info;
	std::vector<DirectionalLight*> directLights;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
	uint maxDirectLight;
	uint maxPointLight;
	uint maxSpotLight;
	std::unique_ptr<vbyte[]> lightData;

public:
	LightController(uint maxDirect, uint maxPoint, uint maxSpot);
	void SetEyePosition(XMFLOAT3 position);
	bool AddLight(PointLight*);
	bool AddLight(DirectionalLight*);
	bool AddLight(SpotLight*);
	bool UpdateLight(DirectionalLight*, uint);
	const vbyte* GetLightConstantBuffer();
	size_t GetSize();
};