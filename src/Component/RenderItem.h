#pragma once
#include <Resource/Mesh.h>
class RenderItem {
private:
	std::string itemName;
	std::unique_ptr<Mesh> mesh;
	std::string shaderFileName;
	std::string diffuseTextureName;
	struct MaterialInfo {
	};

public:
	RenderItem(const char* name);

	Mesh* GetMesh();
	std::string GetName() { return itemName; }
};