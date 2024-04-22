
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_loader.h"
#include <unordered_set>
using namespace DirectX;

struct Position : public rtti::Struct {
	rtti::Var<DirectX::XMFLOAT3> position = "POSITION";
} position;
struct Normal : public rtti::Struct {
	rtti::Var<DirectX::XMFLOAT3> normal = "NORMAL";
} normal;
struct Texcoord : public rtti::Struct {
	rtti::Var<DirectX::XMFLOAT2> texcoord = "TEXCOORD";
} texcoord;

std::string parsePath(const std::string& filepath) {
	size_t found = filepath.find_last_of("/\\");
	if (found != std::string::npos) {
		return filepath.substr(0, found + 1);
	}
	return "";
}

std::string ParseTextureName(const std::string& texturePath) {

	std::filesystem::path pathObj(texturePath);
	std::string filename = pathObj.filename().string();

	// 去除文件扩展名
	size_t lastindex = filename.find_last_of(".");
	return filename.substr(0, lastindex);
}
// //Asume triangle base
// Mesh* LoadMeshFromFile(const std::string& full_path_file, Device* device, ID3D12GraphicsCommandList* cmdList, Log* log) {
// 	tinyobj::ObjReaderConfig reader_config;
// 	reader_config.mtl_search_path = parsePath(full_path_file);
// 	tinyobj::ObjReader reader;
// 	if (!reader.ParseFromFile(full_path_file, reader_config)) {
// 		if (!reader.Error().empty()) {
// 			AddLog(log, reader.Error().c_str());
// 			AddLog(log, "Cannot open obj file!");
// 			return false;
// 		}
// 	}
// 	if (!reader.Warning().empty()) {
// 		AddLog(log, reader.Warning().c_str());
// 	}
// 	tinyobj::attrib_t attrib = reader.GetAttrib();
// 	std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
// 	std::vector<tinyobj::material_t> materials = reader.GetMaterials();

// 	uint countIndices = 0;
// 	uint countVertices = 0;
// 	for (size_t s = 0; s < shapes.size(); s++) {
// 		uint indexCount = shapes[s].mesh.indices.size();
// 		countIndices += indexCount;
// 		countVertices += 3 * shapes[s].mesh.num_face_vertices.size();
// 	}
// 	countIndices = countVertices;
// 	std::vector<uint> indices(countIndices);
// 	std::vector<rtti::Struct const*> structs = {&position, &normal, &texcoord};
// 	std::vector<std::vector<vbyte>> verticesData = {std::vector<vbyte>(structs[0]->structSize * countVertices),
// 													std::vector<vbyte>(structs[1]->structSize * countVertices),
// 													std::vector<vbyte>(structs[2]->structSize * countVertices)};

// 	size_t shape_offset = 0;
// 	for (size_t s = 0; s < shapes.size(); s++) {
// 		size_t index_offset = 0;
// 		size_t faceCount = shapes[s].mesh.num_face_vertices.size();
// 		for (size_t f = 0; f < faceCount; f++) {
// 			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
// 			for (size_t v = 0; v < fv; v++) {
// 				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
// 				//indices[shape_offset + index_offset + v] = idx.vertex_index;
// 				indices[shape_offset + index_offset + v] = shape_offset + index_offset + v;
// 				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
// 				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
// 				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
// 				XMFLOAT3 vert(vx, vy, vz);
// 				*reinterpret_cast<XMFLOAT3*>(verticesData[0].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = vert;

// 				if (idx.normal_index >= 0) {
// 					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
// 					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
// 					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
// 					XMFLOAT3 norm(nx, ny, nz);
// 					*reinterpret_cast<XMFLOAT3*>(verticesData[1].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = norm;//Not wrong here
// 				}
// 				if (idx.texcoord_index >= 0) {
// 					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
// 					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
// 					XMFLOAT2 uv(tx, ty);
// 					*reinterpret_cast<XMFLOAT2*>(verticesData[2].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT2)) = uv;
// 				}
// 			}
// 			index_offset += fv;
// 		}
// 		shape_offset += index_offset;
// 	}

// Mesh* mesh = new Mesh(device, structs, countVertices, countIndices);
// for (size_t i = 0; i < structs.size(); i++) {
// 	mesh->tmpVertexBuffers[i].CopyData(0, {verticesData[i].data(), countVertices * structs[i]->structSize});
// 	cmdList->CopyBufferRegion(
// 		mesh->VertexBuffers()[i].GetResource(),
// 		0,
// 		mesh->tmpVertexBuffers[i].GetResource(),
// 		0,
// 		mesh->tmpVertexBuffers[i].GetByteSize());
// }
// mesh->tmpIndexBuffer.CopyData(0, {reinterpret_cast<vbyte const*>(indices.data()), countIndices * sizeof(uint)});
// // Copy index buffer to mesh
// cmdList->CopyBufferRegion(
// 	mesh->IndexBuffer().GetResource(),
// 	0,
// 	mesh->tmpIndexBuffer.GetResource(),
// 	0,
// 	mesh->tmpIndexBuffer.GetByteSize());

//return mesh;
//}
// bool BuildRenderItemsFromFile0(const std::string& full_path_file, Device* device, ID3D12GraphicsCommandList* cmdList, Log* log, std::unordered_map<std::string, RenderItem*>& map, std::vector<std::string>& loadedItem) {
// 	tinyobj::ObjReaderConfig reader_config;
// 	reader_config.mtl_search_path = parsePath(full_path_file);
// 	tinyobj::ObjReader reader;
// 	if (!reader.ParseFromFile(full_path_file, reader_config)) {
// 		if (!reader.Error().empty()) {
// 			AddLog(log, reader.Error().c_str());
// 			AddLog(log, "Cannot open obj file!");
// 			return false;
// 		}
// 	}
// 	if (!reader.Warning().empty()) {
// 		AddLog(log, reader.Warning().c_str());
// 	}
// 	tinyobj::attrib_t attrib = reader.GetAttrib();
// 	std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
// 	std::vector<tinyobj::material_t> materials = reader.GetMaterials();
// 	std::vector<rtti::Struct const*> structs = {&position, &normal, &texcoord};

// 	for (size_t s = 0; s < shapes.size(); s++) {
// 		std::string itemName = shapes[s].name;
// 		if (map.find(itemName) != map.end()) {
// 			continue;
// 		}
// 		tinyobj::mesh_t tiny_mesh = shapes[s].mesh;
// 		size_t faceCount = tiny_mesh.num_face_vertices.size();
// 		//Don't know why obj file broken when use combined vertices, later solve the issue
// 		uint countVertices = 3 * faceCount;
// 		uint countIndices = countVertices;
// 		std::vector<std::vector<vbyte>> verticesData = {std::vector<vbyte>(structs[0]->structSize * countVertices),
// 														std::vector<vbyte>(structs[1]->structSize * countVertices),
// 														std::vector<vbyte>(structs[2]->structSize * countVertices)};
// 		std::vector<uint> indices(countIndices);
// 		size_t index_offset = 0;
// 		for (size_t f = 0; f < faceCount; f++) {
// 			for (size_t v = 0; v < 3; v++) {
// 				tinyobj::index_t idx = tiny_mesh.indices[index_offset + v];
// 				indices[index_offset + v] = index_offset + v;
// 				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
// 				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
// 				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
// 				XMFLOAT3 vert(vx, vy, vz);
// 				*reinterpret_cast<XMFLOAT3*>(verticesData[0].data() + (index_offset + v) * sizeof(XMFLOAT3)) = vert;

// 				if (idx.normal_index >= 0) {
// 					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
// 					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
// 					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
// 					XMFLOAT3 norm(nx, ny, nz);
// 					*reinterpret_cast<XMFLOAT3*>(verticesData[1].data() + (index_offset + v) * sizeof(XMFLOAT3)) = norm;//Not wrong here
// 				}
// 				if (idx.texcoord_index >= 0) {
// 					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
// 					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
// 					XMFLOAT2 uv(tx, ty);
// 					*reinterpret_cast<XMFLOAT2*>(verticesData[2].data() + (index_offset + v) * sizeof(XMFLOAT2)) = uv;
// 				}
// 			}//end vertex loop
// 			index_offset += 3;
// 		}//end face loop

// 		Mesh* mesh = new Mesh(device, structs, countVertices, countIndices);
// 		for (size_t i = 0; i < structs.size(); i++) {
// 			mesh->tmpVertexBuffers[i].CopyData(0, {verticesData[i].data(), countVertices * structs[i]->structSize});
// 			cmdList->CopyBufferRegion(
// 				mesh->VertexBuffers()[i].GetResource(),
// 				0,
// 				mesh->tmpVertexBuffers[i].GetResource(),
// 				0,
// 				mesh->tmpVertexBuffers[i].GetByteSize());
// 		}
// 		mesh->tmpIndexBuffer.CopyData(0, {reinterpret_cast<vbyte const*>(indices.data()), countIndices * sizeof(uint)});
// 		// Copy index buffer to mesh
// 		cmdList->CopyBufferRegion(
// 			mesh->IndexBuffer().GetResource(),
// 			0,
// 			mesh->tmpIndexBuffer.GetResource(),
// 			0,
// 			mesh->tmpIndexBuffer.GetByteSize());
// 		map[itemName] = new RenderItem(itemName.c_str(), mesh);
// 		loadedItem.push_back(itemName);

// 	}//end shape loop

// 	return true;
// }
bool BuildRenderItemsFromFile1(const std::string& full_path_file,
							   Device* device,
							   ID3D12GraphicsCommandList* cmdList,
							   Log* log,
							   FrameResource* frameRes,
							   std::unordered_map<std::string, RenderItem*>& map_renderItems,
							   std::vector<std::string>& loadedItem,
							   std::unordered_map<std::string, Texture*>& map_textures,
							   std::unordered_map<std::string, MaterialInfo*>& map_materials,
							   DescriptorHeap* srvHeap) {
	tinyobj::ObjReaderConfig reader_config;
	std::string material_search_path = parsePath(full_path_file);
	reader_config.mtl_search_path = material_search_path;
	tinyobj::ObjReader reader;
	if (!reader.ParseFromFile(full_path_file, reader_config)) {
		if (!reader.Error().empty()) {
			AddLog(log, reader.Error().c_str());
			AddLog(log, "Cannot open obj file!");
			return false;
		}
	}
	if (!reader.Warning().empty()) {
		AddLog(log, reader.Warning().c_str());
	}
	tinyobj::attrib_t attrib = reader.GetAttrib();
	std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
	std::vector<tinyobj::material_t> materials = reader.GetMaterials();
	std::vector<rtti::Struct const*> structs = {&position, &normal, &texcoord};

	//Load Material and textures
	for (const auto& material : materials) {
		if (map_materials.find(material.name) != map_materials.end()) {
			continue;
		}
		MaterialInfo* mInfo = new MaterialInfo();
		mInfo->name = material.name;
		mInfo->diffuseTextureName = ParseTextureName(material.diffuse_texname);
		mInfo->bumpTextureName = ParseTextureName(material.bump_texname);
		mInfo->specTextureName = ParseTextureName(material.specular_texname);
		mInfo->maskTextureName = ParseTextureName(material.alpha_texname);
		map_materials[mInfo->name] = mInfo;

		if (!material.diffuse_texname.empty() && map_textures.find(mInfo->diffuseTextureName) == map_textures.end()) {
			map_textures[mInfo->diffuseTextureName] = Load2DTextureFromFile1(device, cmdList, (material_search_path + material.diffuse_texname).c_str(), frameRes);
			map_textures[mInfo->diffuseTextureName]->CreateSRV(srvHeap, 0);
			AddLog(log, std::string("Loading " + material.diffuse_texname + "\n").c_str());
		}
		if (!material.bump_texname.empty() && map_textures.find(mInfo->bumpTextureName) == map_textures.end()) {
			map_textures[mInfo->bumpTextureName] = Load2DTextureFromFile1(device, cmdList, (material_search_path + material.bump_texname).c_str(), frameRes);
			map_textures[mInfo->bumpTextureName]->CreateSRV(srvHeap, 0);
			AddLog(log, std::string("Loading " + material.bump_texname + "\n").c_str());
		}
		if (!material.specular_texname.empty() && map_textures.find(mInfo->specTextureName) == map_textures.end()) {
			map_textures[mInfo->specTextureName] = Load2DTextureFromFile1(device, cmdList, (material_search_path + material.specular_texname).c_str(), frameRes);
			map_textures[mInfo->specTextureName]->CreateSRV(srvHeap, 0);
			AddLog(log, std::string("Loading " + material.specular_texname + "\n").c_str());
		}
		if (!material.alpha_texname.empty() && map_textures.find(mInfo->maskTextureName) == map_textures.end()) {
			map_textures[mInfo->maskTextureName] = Load2DBitFromFile(device, cmdList, (material_search_path + material.alpha_texname).c_str(), frameRes);
			map_textures[mInfo->maskTextureName]->CreateSRV(srvHeap, 0);
			AddLog(log, std::string("Loading " + material.alpha_texname + "\n").c_str());
		}
	}//end Material and Texture Loop

	for (size_t s = 0; s < shapes.size(); s++) {
		std::string itemName = shapes[s].name;
		if (map_renderItems.find(itemName) != map_renderItems.end()) {
			continue;
		}
		tinyobj::mesh_t tiny_mesh = shapes[s].mesh;
		size_t faceCount = tiny_mesh.num_face_vertices.size();
		//Don't know why obj file broken when use combined vertices, later solve the issue
		uint countVertices = 3 * faceCount;
		uint countIndices = countVertices;
		std::vector<std::vector<vbyte>> verticesData = {std::vector<vbyte>(structs[0]->structSize * countVertices),
														std::vector<vbyte>(structs[1]->structSize * countVertices),
														std::vector<vbyte>(structs[2]->structSize * countVertices)};
		std::vector<uint> indices(countIndices);
		size_t index_offset = 0;
		for (size_t f = 0; f < faceCount; f++) {
			for (size_t v = 0; v < 3; v++) {
				tinyobj::index_t idx = tiny_mesh.indices[index_offset + v];
				indices[index_offset + v] = index_offset + v;
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				XMFLOAT3 vert(vx, vy, vz);
				*reinterpret_cast<XMFLOAT3*>(verticesData[0].data() + (index_offset + v) * sizeof(XMFLOAT3)) = vert;

				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					XMFLOAT3 norm(nx, ny, nz);
					*reinterpret_cast<XMFLOAT3*>(verticesData[1].data() + (index_offset + v) * sizeof(XMFLOAT3)) = norm;//Not wrong here
				}
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = 1 - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					XMFLOAT2 uv(tx, ty);
					*reinterpret_cast<XMFLOAT2*>(verticesData[2].data() + (index_offset + v) * sizeof(XMFLOAT2)) = uv;
				}
			}//end vertex loop
			index_offset += 3;
		}//end face loop
		std::string materialName = "";
		int material_id = tiny_mesh.material_ids[0];
		if (material_id >= 0) {
			materialName = materials[material_id].name;
		}
		Mesh* mesh = new Mesh(device, structs, countVertices, countIndices, frameRes, cmdList, verticesData, indices);
		map_renderItems[itemName] = new RenderItem(itemName.c_str(), mesh, materialName);

		loadedItem.push_back(itemName);
	}//end shape loop

	return true;
}