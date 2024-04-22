#include <Resource/Mesh.h>

Mesh::Mesh(Device* device,
		   std::vector<rtti::Struct const*> vbStructs,
		   uint vertexCount,
		   uint indexCount,
		   FrameResource* frameRes,
		   ID3D12GraphicsCommandList* cmdList,
		   std::vector<std::vector<vbyte>>& vertexDatas,
		   std::vector<uint>& indexData)
	: Resource(device),
	  vertexStructs(vbStructs),
	  vertexCount(vertexCount),
	  indexCount(indexCount),
	  indexBuffer(device, sizeof(uint) * indexCount) {

	vertexBuffers.reserve(vbStructs.size());
	uint slotCount = 0;
	for (size_t i = 0; i < vbStructs.size(); i++) {
		size_t bufferSize = vertexCount * vbStructs[i]->structSize;
		vertexBuffers.emplace_back(device, bufferSize);
		vbStructs[i]->GetMeshLayout(slotCount, layout);
		++slotCount;
		UploadBuffer tmpVerUB(device, bufferSize);
		tmpVerUB.CopyData(0, {vertexDatas[i].data(), bufferSize});
		cmdList->CopyBufferRegion(
			vertexBuffers[i].GetResource(),
			0,
			tmpVerUB.GetResource(),
			0,
			tmpVerUB.GetByteSize());
		frameRes->AddDelayDisposeResource(tmpVerUB.GetResource());
	}
	UploadBuffer tmpIndUB(device, sizeof(uint) * indexCount);
	tmpIndUB.CopyData(0, {reinterpret_cast<vbyte const*>(indexData.data()), indexCount * sizeof(uint)});
	// Copy index buffer to mesh
	cmdList->CopyBufferRegion(
		indexBuffer.GetResource(),
		0,
		tmpIndUB.GetResource(),
		0,
		tmpIndUB.GetByteSize());
	frameRes->AddDelayDisposeResource(tmpIndUB.GetResource());
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