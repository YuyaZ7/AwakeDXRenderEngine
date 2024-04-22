#include "PointLight.h"
uint PointLight::GetRawSize() {
	return sizeof(RawData);
}

PointLight::RawData PointLight::GetRawData() {
	return RawData{
		.Strength = this->intensity,
		.padding0 = 0,
		.Position = this->position,
		.padding1 = 0};
}