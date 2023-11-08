//*********************************************************
//0
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "DXSample.h"
#include <Resource/Mesh.h>
#include <DXRuntime/Device.h>
#include <Resource/UploadBuffer.h>
#include <Resource/Texture.h>
#include <Resource/DescriptorHeap.h>
#include <DXRuntime/ResourceStateTracker.h>
#include <DXRuntime/BindProperty.h>
#include <iostream>
#include <unordered_map>
#include <commdlg.h>
#include <future>
#include <Component/Log.h>
#include <Component/stb_loader.h>
#include <Component/LightController.h>
#include <Component/Camera.h>
#include <Component/GameTimer.h>
#include <Component/RenderItem.h>
using namespace DirectX;
// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;
class FrameResource;
class Camera;
class PSOManager;
class RasterShader;
class CommandListHandle;
class DefaultBuffer;

class D3D12BetterSimpleBox : public DXSample {
public:
	D3D12BetterSimpleBox(uint32_t width, uint32_t height, std::wstring name);
	D3D12BetterSimpleBox(D3D12BetterSimpleBox const&) = delete;
	D3D12BetterSimpleBox(D3D12BetterSimpleBox&&) = delete;
	void OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDestroy() override;
	void OnKeyDown(UINT8 /*key*/) override;
	void OnKeyUp(UINT8 /*key*/) override;
	void OnMouseDown(WPARAM, int32 /*x*/, int32 /*y*/) override;
	void OnMouseUp(WPARAM, int32 /*x*/, int32 /*y*/) override;
	void OnMouseMove(WPARAM, int32 /*x*/, int32 /*y*/) override;
	~D3D12BetterSimpleBox();

private:
	int32 mouse_last_x, mouse_last_y = 0;
	static const uint32_t FrameCount = 3;
	static const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	std::unique_ptr<Device> device;
	std::unique_ptr<Camera> mainCamera;
	std::unique_ptr<LightController> lightController;
	// Pipeline objects.
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	std::unordered_map<std::string, RenderItem*> m_renderItems;
	std::vector<std::string> m_frameRenderItems;
	std::unique_ptr<Texture> m_renderTargets[FrameCount];
	std::unique_ptr<Texture> m_depthTargets[FrameCount];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	std::vector<std::future<std::string>> m_loadFileName;
	std::unique_ptr<Log> log;
	std::unordered_map<std::string, Mesh*> m_Meshes;
	std::unique_ptr<DescriptorHeap> m_rtvHeap;
	std::unique_ptr<DescriptorHeap> m_dsvHeap;
	std::unique_ptr<DescriptorHeap> m_imgui_srvHeap;
	std::unique_ptr<DescriptorHeap> m_cbv_srv_uavHeap;
	std::unordered_map<std::string, Texture*> m_textures;
	std::unique_ptr<UploadBuffer> m_uploadBuffer;
	std::unique_ptr<UploadBuffer> m_another_uploadBuffer;
	std::unique_ptr<PSOManager> psoManager;
	std::unique_ptr<RasterShader> colorShader;
	uint32_t m_rtvDescriptorSize;
	uint32_t m_dsvDescriptorSize;
	uint32_t m_srvDescriptorSize;
	std::unique_ptr<FrameResource> frameResources[FrameCount];
	ResourceStateTracker stateTracker;
	// App resources.
	std::unique_ptr<Mesh> triangleMesh;
	// Synchronization objects.
	uint32_t m_backBufferIndex;
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_fenceValue;
	std::vector<BindProperty> bindProperties;
	std::unique_ptr<GameTimer> m_timer;
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList(FrameResource& frameRes, uint frameIndex);
};
