#pragma once
#include <Resource/Mesh.h>
#include <Component/Material.h>
class RenderItem {
private:
	std::string itemName;
	std::string materilName;
	std::unique_ptr<Mesh> mesh;
	std::string shaderName;
	MaterialInfo* material;

public:
	RenderItem(const char* name, Mesh* mesh, std::string materialName);

	Mesh* GetMesh() const { return mesh.get(); }
	std::string GetName() const { return itemName; }
	std::string GetMaterialName() const { return materilName; }
};