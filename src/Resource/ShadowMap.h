#pragma once
#include <Resource/Resource.h>
#include <Resource/DescriptorHeap.h>
class ShadowMap : public Resource {
public:
	ShadowMap(
		Device* device,
		uint width,
		uint height,
		DXGI_FORMAT format,
		uint mip,
		D3D12_RESOURCE_STATES resourceState);
	~ShadowMap();

	ID3D12Resource* GetResource() const override {
		return resource.Get();
	}
	D3D12_RESOURCE_STATES GetInitState() const {
		return initState;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC* GetSrvDesc(uint mipOffset) const;
	D3D12_DEPTH_STENCIL_VIEW_DESC* GetDsvDesc(uint targetMipLevel) const;
	void DelayDispose(FrameResource* frameRes) const override;
	void CreateSRV(DescriptorHeap*, uint);
	void CreateDSV(DescriptorHeap*, uint);
	void DeleteSRV();
	DescriptorHeap* GetSRVDescriptorHeap() { return srv_heap; };
	DescriptorHeap* GetDSVDescriptorHeap() { return dsv_heap; };
	uint GetSRViewIndex() { return viewIndex_SRV; };
	uint GetDSViewIndex() { return viewIndex_DSV; };

private:
	ComPtr<ID3D12Resource> resource;
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = nullptr;
	uint viewIndex_SRV;
	uint viewIndex_DSV;
	DescriptorHeap* srv_heap;
	DescriptorHeap* dsv_heap;
	D3D12_RESOURCE_STATES initState;
};
