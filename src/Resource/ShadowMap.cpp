#include "ShadowMap.h"
#include <DXRuntime/FrameResource.h>

ShadowMap::ShadowMap(Device* device,
					 uint width,
					 uint height,
					 DXGI_FORMAT format,
					 uint mip,
					 D3D12_RESOURCE_STATES resourceState) : Resource(device), initState(resourceState) {
	D3D12_RESOURCE_DESC smDesc;
	memset(&smDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	smDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	smDesc.Alignment = 0;
	smDesc.Width = width;
	smDesc.Height = height;
	smDesc.DepthOrArraySize = 1;
	smDesc.MipLevels = mip;
	smDesc.Format = format;
	smDesc.SampleDesc.Count = 1;
	smDesc.SampleDesc.Quality = 0;
	smDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	smDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);//Default since only gpu read/write
	D3D12_HEAP_PROPERTIES const* propPtr = &prop;
	D3D12_CLEAR_VALUE clearValue;

	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	clearValue.Format = format;

	ThrowIfFailed(device->DxDevice()->CreateCommittedResource(
		propPtr,
		D3D12_HEAP_FLAG_NONE,
		&smDesc,
		initState,
		&clearValue,
		IID_PPV_ARGS(&resource)));

	srvDesc = new D3D12_SHADER_RESOURCE_VIEW_DESC;
	dsvDesc = new D3D12_DEPTH_STENCIL_VIEW_DESC;
}

void ShadowMap::CreateSRV(DescriptorHeap* descriptorHeap, uint mipLevels) {
	srv_heap = descriptorHeap;
	viewIndex_SRV = descriptorHeap->AllocateIndex();
	GetSrvDesc(mipLevels);
	descriptorHeap->CreateSRV(GetResource(), *srvDesc, viewIndex_SRV);
}

void ShadowMap::CreateDSV(DescriptorHeap* descriptorHeap, uint mipLevels) {
	dsv_heap = descriptorHeap;
	viewIndex_DSV = descriptorHeap->AllocateIndex();
	GetDsvDesc(mipLevels);
	descriptorHeap->CreateDSV(GetResource(), *dsvDesc, viewIndex_DSV);
}

D3D12_SHADER_RESOURCE_VIEW_DESC* ShadowMap::GetSrvDesc(uint mipOffset) const {
	srvDesc->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	auto format = resource->GetDesc();
	srvDesc->Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc->Texture2D.MostDetailedMip = 0;
	srvDesc->Texture2D.MipLevels = format.MipLevels;
	srvDesc->Texture2D.PlaneSlice = 0;
	srvDesc->Texture2D.ResourceMinLODClamp = 0.0f;

	return srvDesc;
}
D3D12_DEPTH_STENCIL_VIEW_DESC* ShadowMap::GetDsvDesc(uint mipOffset) const {

	auto format = resource->GetDesc();
	dsvDesc->Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc->Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc->Texture2D.MipSlice = 0;

	return dsvDesc;
}

ShadowMap::~ShadowMap() {
	delete srvDesc;
	delete dsvDesc;
}

void ShadowMap::DelayDispose(FrameResource* frameRes) const {
	frameRes->AddDelayDisposeResource(resource);
}