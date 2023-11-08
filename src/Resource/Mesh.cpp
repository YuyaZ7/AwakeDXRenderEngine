#include <Resource/Mesh.h>
Mesh::Mesh(
	Device* device,
	std::vector<rtti::Struct const*> vbStruct,
	uint vertexCount,
	uint indexCount)
	: Resource(device),
	  vertexStructs(vbStruct),
	  vertexCount(vertexCount),
	  indexCount(indexCount),
	  indexBuffer(device, sizeof(uint) * indexCount),
	  tmpIndexBuffer(device, sizeof(uint) * indexCount) {
	vertexBuffers.reserve(vertexStructs.size());
	tmpVertexBuffers.reserve(vertexStructs.size());
	uint slotCount = 0;
	for (uint i = 0; i < vertexStructs.size(); i++) {
		tmpVertexBuffers.emplace_back(device, vertexStructs[i]->structSize * vertexCount);
		vertexBuffers.emplace_back(device, vertexStructs[i]->structSize * vertexCount);
		vertexStructs[i]->GetMeshLayout(slotCount, layout);
		++slotCount;
	}
}
void Mesh::GetVertexBufferView(std::vector<D3D12_VERTEX_BUFFER_VIEW>& result) const {
	result.clear();
	result.resize(vertexBuffers.size());
	for (size_t i = 0; i < vertexBuffers.size(); ++i) {
		auto& r = result[i];
		auto& v = vertexBuffers[i];
		r.BufferLocation = v.GetAddress();
		r.SizeInBytes = v.GetByteSize();
		r.StrideInBytes = r.SizeInBytes / vertexCount;
	}
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW v;
	v.BufferLocation = indexBuffer.GetAddress();
	v.SizeInBytes = indexBuffer.GetByteSize();
	v.Format = DXGI_FORMAT_R32_UINT;
	return v;
}