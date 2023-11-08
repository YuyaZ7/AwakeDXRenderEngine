#pragma once
#include <DXMath/DXMath.h>
#include <DXMath/MathHelper.h>
#include "Metalib.h"
#include <span>
struct PointLight {
	PointLight(XMFLOAT3 intensity, XMFLOAT3 position)
		: intensity(intensity), padding0(0), position(position), padding1(0){};
	XMFLOAT3 intensity;
	uint padding0;//memory alignment
	XMFLOAT3 position;
	uint padding1;
};
struct DirectLight {
	DirectLight(XMFLOAT3 intensity, XMFLOAT3 direction)
		: intensity(intensity), padding0(0), direction(direction), padding1(0){};
	XMFLOAT3 intensity;
	uint padding0;
	XMFLOAT3 direction;
	uint padding1;
};

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
	std::vector<PointLight> pointLights;
	std::vector<DirectLight> directLights;
	uint maxDirectLight;
	uint maxPointLight;
	std::unique_ptr<vbyte[]> lightData;

public:
	LightController(uint maxDirect, uint maxPoint);
	void SetEyePosition(XMFLOAT3 position);
	bool AddLight(PointLight);
	bool AddLight(DirectLight);
	const vbyte* GetLightConstantBuffer();
	size_t GetSize();
};