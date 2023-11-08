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
//Asume triangle base
Mesh* LoadMeshFromFile(const std::string& full_path_file, Device* device, ID3D12GraphicsCommandList* cmdList, Log* log) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, full_path_file.c_str());

	if (!warn.empty()) {
		AddLog(log, warn.data());
	}

	if (!err.empty()) {
		AddLog(log, err.data());
	}
	if (!ret) {
		AddLog(log, "No return data from tiny_loader.");
		return nullptr;
	}
	uint countIndices = 0;
	uint countVertices = 0;
	for (size_t s = 0; s < shapes.size(); s++) {
		uint indexCount = shapes[s].mesh.indices.size();
		countIndices += indexCount;
		countVertices += 3 * shapes[s].mesh.num_face_vertices.size();
	}
	countIndices = countVertices;
	std::vector<uint> indices(countIndices);
	std::vector<rtti::Struct const*> structs = {&position, &normal, &texcoord};
	std::vector<std::vector<vbyte>> verticesData = {std::vector<vbyte>(structs[0]->structSize * countVertices),
													std::vector<vbyte>(structs[1]->structSize * countVertices),
													std::vector<vbyte>(structs[2]->structSize * countVertices)};

	size_t shape_offset = 0;
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;
		size_t faceCount = shapes[s].mesh.num_face_vertices.size();
		for (size_t f = 0; f < faceCount; f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				//indices[shape_offset + index_offset + v] = idx.vertex_index;
				indices[shape_offset + index_offset + v] = shape_offset + index_offset + v;
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				XMFLOAT3 vert(vx, vy, vz);
				*reinterpret_cast<XMFLOAT3*>(verticesData[0].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = vert;

				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					XMFLOAT3 norm(nx, ny, nz);
					*reinterpret_cast<XMFLOAT3*>(verticesData[1].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = norm;//Not wrong here
				}
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					XMFLOAT2 uv(tx, ty);
					*reinterpret_cast<XMFLOAT2*>(verticesData[2].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT2)) = uv;
				}
			}
			index_offset += fv;
		}
		shape_offset += index_offset;
	}

	Mesh* mesh = new Mesh(device, structs, countVertices, countIndices);
	for (size_t i = 0; i < structs.size(); i++) {
		mesh->tmpVertexBuffers[i].CopyData(0, {verticesData[i].data(), countVertices * structs[i]->structSize});
		cmdList->CopyBufferRegion(
			mesh->VertexBuffers()[i].GetResource(),
			0,
			mesh->tmpVertexBuffers[i].GetResource(),
			0,
			mesh->tmpVertexBuffers[i].GetByteSize());
	}
	mesh->tmpIndexBuffer.CopyData(0, {reinterpret_cast<vbyte const*>(indices.data()), countIndices * sizeof(uint)});
	// Copy index buffer to mesh
	cmdList->CopyBufferRegion(
		mesh->IndexBuffer().GetResource(),
		0,
		mesh->tmpIndexBuffer.GetResource(),
		0,
		mesh->tmpIndexBuffer.GetByteSize());

	return mesh;
}
bool ReadFile(const std::string& full_path_file, Device* device, ID3D12GraphicsCommandList* cmdList, Log* log, std::unordered_map<std::string, RenderItem> map) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, full_path_file.c_str());

	if (!warn.empty()) {
		AddLog(log, warn.data());
	}

	if (!err.empty()) {
		AddLog(log, err.data());
	}
	if (!ret) {
		AddLog(log, "No return data from tiny_loader.");
		return false;
	}
	uint countIndices = 0;
	uint countVertices = 0;
	for (size_t s = 0; s < shapes.size(); s++) {
		uint indexCount = shapes[s].mesh.indices.size();
		countIndices += indexCount;
		countVertices += 3 * shapes[s].mesh.num_face_vertices.size();
	}
	countIndices = countVertices;
	std::vector<uint> indices(countIndices);
	std::vector<rtti::Struct const*> structs = {&position, &normal, &texcoord};
	std::vector<std::vector<vbyte>> verticesData = {std::vector<vbyte>(structs[0]->structSize * countVertices),
													std::vector<vbyte>(structs[1]->structSize * countVertices),
													std::vector<vbyte>(structs[2]->structSize * countVertices)};

	size_t shape_offset = 0;
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;
		size_t faceCount = shapes[s].mesh.num_face_vertices.size();
		for (size_t f = 0; f < faceCount; f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				//indices[shape_offset + index_offset + v] = idx.vertex_index;
				indices[shape_offset + index_offset + v] = shape_offset + index_offset + v;
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				XMFLOAT3 vert(vx, vy, vz);
				*reinterpret_cast<XMFLOAT3*>(verticesData[0].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = vert;

				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					XMFLOAT3 norm(nx, ny, nz);
					*reinterpret_cast<XMFLOAT3*>(verticesData[1].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT3)) = norm;//Not wrong here
				}
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					XMFLOAT2 uv(tx, ty);
					*reinterpret_cast<XMFLOAT2*>(verticesData[2].data() + (shape_offset + index_offset + v) * sizeof(XMFLOAT2)) = uv;
				}
			}
			index_offset += fv;
		}
		shape_offset += index_offset;
	}

	Mesh* mesh = new Mesh(device, structs, countVertices, countIndices);
	for (size_t i = 0; i < structs.size(); i++) {
		mesh->tmpVertexBuffers[i].CopyData(0, {verticesData[i].data(), countVertices * structs[i]->structSize});
		cmdList->CopyBufferRegion(
			mesh->VertexBuffers()[i].GetResource(),
			0,
			mesh->tmpVertexBuffers[i].GetResource(),
			0,
			mesh->tmpVertexBuffers[i].GetByteSize());
	}
	mesh->tmpIndexBuffer.CopyData(0, {reinterpret_cast<vbyte const*>(indices.data()), countIndices * sizeof(uint)});
	// Copy index buffer to mesh
	cmdList->CopyBufferRegion(
		mesh->IndexBuffer().GetResource(),
		0,
		mesh->tmpIndexBuffer.GetResource(),
		0,
		mesh->tmpIndexBuffer.GetByteSize());

	return mesh;
}