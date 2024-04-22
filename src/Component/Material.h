#pragma once
#include <string>
struct PBRMaterial {
	float baseColor;
	float metallic;
	float specular;
	float roughness;
};
struct MaterialInfo {
	std::string name;

	std::string diffuseTextureName;
	std::string bumpTextureName;
	std::string specTextureName;
	std::string maskTextureName;
};