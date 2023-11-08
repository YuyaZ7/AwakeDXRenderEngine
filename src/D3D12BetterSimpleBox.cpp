//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "Metalib.h"
#include "stdafx.h"
#include "D3D12BetterSimpleBox.h"
#include <DXRuntime/FrameResource.h>
#include <Resource/DefaultBuffer.h>
#include <Shader/RasterShader.h>
#include <Shader/PSOManager.h>

D3D12BetterSimpleBox::D3D12BetterSimpleBox(uint32_t width, uint32_t height, std::wstring name)
	: DXSample(width, height, name),
	  m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	  m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)) {
}

// Load the rendering pipeline dependencies.
void D3D12BetterSimpleBox::LoadPipeline() {

	log = std::make_unique<Log>();
	log->AutoScroll = true;
	AddLog(log.get(), "The Log\n");

	// create device;
	device = std::make_unique<Device>();
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device->DxDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = rtvFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(device->DxgiFactory()->CreateSwapChainForHwnd(
		m_commandQueue.Get(),// Swap chain needs the queue so that it can force a flush on it.
		Win32Application::GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(device->DxgiFactory()->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&m_swapChain));
	m_backBufferIndex = 0;

	//Create Descriptor Heaps.
	m_rtvDescriptorSize = device->DxDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = device->DxDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_srvDescriptorSize = device->DxDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//rtv and dsv
	m_rtvHeap = std::unique_ptr<DescriptorHeap>(new DescriptorHeap(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount, false));
	m_dsvHeap = std::unique_ptr<DescriptorHeap>(new DescriptorHeap(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, FrameCount, false));
	//Create a SrvHeap for the imgui
	m_imgui_srvHeap = std::unique_ptr<DescriptorHeap>(new DescriptorHeap(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true));
	//per frame shader resource
	m_cbv_srv_uavHeap = std::unique_ptr<DescriptorHeap>(new DescriptorHeap(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, true));
	//Imgui d3d12 initialize and setup
	ImGui_ImplDX12_Init(device->DxDevice(), FrameCount, rtvFormat, m_imgui_srvHeap->GetHeap(),
						m_imgui_srvHeap->hCPU(0), m_imgui_srvHeap->hGPU(0));

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->hCPU(0));
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->hCPU(0));

		// Create a RTV for each frame.
		for (uint32_t n = 0; n < FrameCount; n++) {
			m_renderTargets[n] = std::unique_ptr<Texture>(new Texture(device.get(), m_swapChain.Get(), n));
			m_depthTargets[n] = std::unique_ptr<Texture>(
				new Texture(
					device.get(),
					m_scissorRect.right,
					m_scissorRect.bottom,
					DXGI_FORMAT_D32_FLOAT,
					TextureDimension::Tex2D,
					1,
					1,
					Texture::TextureUsage::DepthStencil,
					D3D12_RESOURCE_STATE_DEPTH_READ));

			device->DxDevice()->CreateRenderTargetView(m_renderTargets[n]->GetResource(), nullptr, rtvHandle);
			device->DxDevice()->CreateDepthStencilView(m_depthTargets[n]->GetResource(), nullptr, dsvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
			dsvHandle.Offset(1, m_dsvDescriptorSize);
		}
	}
	// Init FrameResources
	for (auto&& i : frameResources) {
		i = std::unique_ptr<FrameResource>(new FrameResource(device.get()));
	}
}
//tiny_loader.cpp
Mesh* LoadMeshFromFile(const std::string& full_path_file, Device* device, ID3D12GraphicsCommandList* cmdList, Log* log);

// Load the sample assets.
void D3D12BetterSimpleBox::LoadAssets() {

	// Create mesh
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ThrowIfFailed(
		device
			->DxDevice()
			->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator.GetAddressOf())));
	ThrowIfFailed(device->DxDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())));
	ThrowIfFailed(commandList->Close());
	ThrowIfFailed(cmdAllocator->Reset());
	ThrowIfFailed(commandList->Reset(cmdAllocator.Get(), nullptr));
	//Build mesh
	//triangleMesh = std::unique_ptr<Mesh>(BuildFromFile("E:/Dev/AwakeDXRenderEngine/res/bunny/bunny.obj", device.get(), commandList.Get()));
	triangleMesh = std::unique_ptr<Mesh>(LoadMeshFromFile("E:/Dev/AwakeDXRenderEngine/res/sponza/sponza.obj", device.get(), commandList.Get(), log.get()));
	//Build Texture
	m_textures["default"] = Load2DTextureFromFile(device.get(), commandList.Get(), "E:/Dev/AwakeDXRenderEngine/res/default.png", m_uploadBuffer.get());
	m_textures["skybox"] = LoadCubeTextureFromFile(device.get(), commandList.Get(), "E:/Dev/AwakeDXRenderEngine/res/landscape-watercolor-sky.png", m_another_uploadBuffer.get());
	device->DxDevice()->CreateShaderResourceView(m_textures["default"]->GetResource(), m_textures["default"]->GetColorSrvDesc(0), m_cbv_srv_uavHeap->hCPU(0));
	device->DxDevice()->CreateShaderResourceView(m_textures["skybox"]->GetResource(), m_textures["skybox"]->GetColorSrvDesc(0), m_cbv_srv_uavHeap->hCPU(1));
	ThrowIfFailed(commandList->Close());

	// Execute CommandList
	ID3D12CommandList* ppCommandLists[] = {commandList.Get()};
	m_commandQueue->ExecuteCommandLists(array_count(ppCommandLists), ppCommandLists);

	//Build Camera
	mainCamera = std::make_unique<Camera>();
	mainCamera->Right = Math::Vector3(0.6877694, -1.622736E-05, 0.7259292);
	mainCamera->Up = Math::Vector3(-0.3181089, 0.8988663, 0.301407);
	mainCamera->Forward = Math::Vector3(-0.6525182, -0.438223, 0.6182076);
	mainCamera->Position = Math::Vector3(2.232773, 1.501817, -1.883978);
	mainCamera->SetAspect(static_cast<float>(m_scissorRect.right) / static_cast<float>(m_scissorRect.bottom));
	mainCamera->UpdateProjectionMatrix();
	mainCamera->UpdateViewMatrix();
	// Build lights
	lightController = std::make_unique<LightController>(4, 4);
	DirectLight light0(XMFLOAT3(1, 1, 1), XMFLOAT3(0, 1, 0));
	PointLight light1(XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, 0));
	lightController->AddLight(light0);
	lightController->AddLight(light1);
	lightController->SetEyePosition(mainCamera->Position);

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		std::vector<std::pair<std::string, Shader::Property>> properties;
		properties.emplace_back(
			"MVPBuffer",
			Shader::Property{
				.type = ShaderVariableType::ConstantBuffer,
				.spaceIndex = 0,
				.registerIndex = 0,
				.arrSize = 0});
		properties.emplace_back(
			"LightBuffer",
			Shader::Property{
				.type = ShaderVariableType::ConstantBuffer,
				.spaceIndex = 0,
				.registerIndex = 1,
				.arrSize = 0});
		properties.emplace_back(
			"baseTexture",
			Shader::Property{
				.type = ShaderVariableType::SRVDescriptorHeap,
				.spaceIndex = 0,
				.registerIndex = 0,
				.arrSize = 0});

		colorShader = std::unique_ptr<RasterShader>(
			new RasterShader(
				properties,
				device.get()));
		psoManager = std::unique_ptr<PSOManager>(
			new PSOManager(device.get()));
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		ComPtr<ID3DBlob> compilationMsgs = nullptr;
#else
		uint32_t compileFlags = 0;
#endif
		//Compile Shaders
		HRESULT compileResult = D3DCompileFromFile(GetAssetFullPath(L"shader/shaders.hlsl").c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &compilationMsgs);
		std::string msg;
		if (FAILED(compileResult)) {
			if (compilationMsgs != nullptr) {
				msg = static_cast<const char*>(compilationMsgs->GetBufferPointer());
			}
			throw HrException(compileResult);
		}
		compileResult = D3DCompileFromFile(GetAssetFullPath(L"shader/shaders.hlsl").c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &compilationMsgs);
		if (FAILED(compileResult)) {
			if (compilationMsgs != nullptr) {
				msg = static_cast<const char*>(compilationMsgs->GetBufferPointer());
			}
			throw HrException(compileResult);
		}
		colorShader->vsShader = std::move(vertexShader);
		colorShader->psShader = std::move(pixelShader);
		colorShader->rasterizeState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		colorShader->blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		auto&& depthStencilState = colorShader->depthStencilState;
		depthStencilState.DepthEnable = true;
		depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilState.StencilEnable = false;
	}
	//Pass skybox
	{
		std::vector<std::pair<std::string, Shader::Property>> properties;
	}
	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ThrowIfFailed(device->DxDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue = 1;
		m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
		if (m_fence->GetCompletedValue() < m_fenceValue) {
			LPCWSTR falseValue = 0;
			HANDLE eventHandle = CreateEventEx(nullptr, falseValue, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}
void D3D12BetterSimpleBox::OnInit() {
	m_timer = std::unique_ptr<GameTimer>(new GameTimer());
	m_timer->Reset();
	LoadPipeline();
	LoadAssets();
}
void D3D12BetterSimpleBox::OnKeyDown(UINT8 key) {
	const float dt = m_timer->DeltaTime();
	float speed = 200.f;
	if (GetAsyncKeyState('W') & 0x8000)
		mainCamera->Walk(speed * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mainCamera->Walk(-speed * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mainCamera->Strafe(-speed * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mainCamera->Strafe(speed * dt);

	mainCamera->UpdateViewMatrix();
	SetFocus(Win32Application::GetHwnd());
}
void D3D12BetterSimpleBox::OnKeyUp(UINT8 key) {
}
void D3D12BetterSimpleBox::OnMouseDown(WPARAM, int32 x, int32 y) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	io.MouseDown[0] = true;
	mouse_last_x = x;
	mouse_last_y = y;
	SetFocus(Win32Application::GetHwnd());
}
void D3D12BetterSimpleBox::OnMouseUp(WPARAM, int32 x, int32 y) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	io.MouseDown[0] = false;
}

void D3D12BetterSimpleBox::OnMouseMove(WPARAM btnState, int32 x, int32 y) {
	bool leftButtonDown = btnState & MK_LBUTTON;
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	io.MouseDown[0] = leftButtonDown;
	if (io.WantCaptureMouse) {

	} else {
		if (btnState & MK_LBUTTON != 0) {

			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.5f * static_cast<float>(x - mouse_last_x));
			float dy = XMConvertToRadians(0.5f * static_cast<float>(y - mouse_last_y));

			mainCamera->Pitch(dy);
			mainCamera->RotateY(dx);
		}
	}

	mouse_last_x = x;
	mouse_last_y = y;
}
std::string OpenFileAsync(Log* log, const wchar_t* filter) {
	wchar_t filename[256];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = filename;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = filter;
	ofn.lpstrTitle = L"Select Model File";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&ofn)) {
		AddLog(log, "Cancel opening file.\n");
	}
	std::wstring ws(filename);
	return std::string(ws.begin(), ws.end());
}
// Update frame-based values.
void D3D12BetterSimpleBox::OnUpdate() {
	m_timer->Tick();
	//Imgui
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Message Example");
		ImGui::Text("This is a regular message.");
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: Something went wrong!");
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: Something went terribly wrong!");
		//Load obj file
		if (ImGui::Button("Load model")) {
			m_loadFileName.emplace_back(std::async(std::launch::async, OpenFileAsync, log.get(), L"Model Files\0*.obj\0\0"));
		}

		ShowLog(log.get());
		ImGui::End();
	}

	OnKeyDown(0);
}
// Render the scene.
void D3D12BetterSimpleBox::OnRender() {
	// Record all the commands we need to render the scene into the command list.
	auto curFrame = m_backBufferIndex;
	auto nextFrame = (curFrame + 1) % FrameCount;
	auto lastFrame = (nextFrame + 1) % FrameCount;
	// Execute and Present
	frameResources[curFrame]->Execute(
		m_commandQueue.Get(),
		m_fence.Get(),
		m_fenceValue);
	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_backBufferIndex = (m_backBufferIndex + 1) % FrameCount;
	// Signal Frame
	frameResources[curFrame]->Signal(
		m_commandQueue.Get(),
		m_fence.Get());
	// Populate next frame
	PopulateCommandList(*frameResources[nextFrame], nextFrame);
	// Sync last frame
	frameResources[lastFrame]->Sync(
		m_fence.Get());
}
void D3D12BetterSimpleBox::OnDestroy() {
	// Sync all frame
	for (auto&& i : frameResources) {
		i->Sync(m_fence.Get());
	}
	ImGui_ImplDX12_Shutdown();
}

void D3D12BetterSimpleBox::PopulateCommandList(FrameResource& frameRes, uint frameIndex) {
	auto cmdListHandle = frameRes.Command();
	auto cmdList = cmdListHandle.CmdList();
	//Load File
	for (auto i = m_loadFileName.begin(); i != m_loadFileName.end();) {
		if (i->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::string fileName = i->get();//Not valid after call
			m_Meshes[fileName] = LoadMeshFromFile(fileName, device.get(), cmdList, log.get());
			i = m_loadFileName.erase(i);
			AddLog(log.get(), "Successful load the file!!! ");
			AddLog(log.get(), fileName.c_str());
		} else if (i->wait_for(std::chrono::seconds(0)) == std::future_status::deferred) {
			AddLog(log.get(), "Deferred!!!\n");
			++i;

		} else if (i->wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
			AddLog(log.get(), "Time Out!!!\n");
			++i;
		}
	}
	ID3D12DescriptorHeap* descriptorHeaps[] = {m_cbv_srv_uavHeap->GetHeap()};
	cmdList->SetDescriptorHeaps(1, descriptorHeaps);
	stateTracker.RecordState(m_renderTargets[frameIndex].get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	stateTracker.RecordState(m_depthTargets[frameIndex].get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
	stateTracker.UpdateState(cmdList);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->hCPU(0), frameIndex, m_rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->hCPU(0), frameIndex, m_dsvDescriptorSize);
	frameRes.SetRenderTarget(
		m_renderTargets[frameIndex].get(),
		&rtvHandle,
		&dsvHandle);
	frameRes.ClearRTV(rtvHandle);
	frameRes.ClearDSV(dsvHandle);
	// Record commands.
	DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;
	mainCamera->UpdateViewMatrix();
	Math::Matrix4 viewProjMatrix = mainCamera->Proj * mainCamera->View;
	auto mvpBuffer = frameRes.AllocateConstBuffer({reinterpret_cast<uint8_t const*>(&viewProjMatrix), sizeof(viewProjMatrix)});

	lightController->SetEyePosition(mainCamera->Position);
	auto lightBuffer = frameRes.AllocateConstBuffer({reinterpret_cast<uint8_t const*>(lightController->GetLightConstantBuffer()), lightController->GetSize()});

	DescriptorHeapView textureView(m_cbv_srv_uavHeap.get(), 0);
	bindProperties.clear();
	bindProperties.emplace_back("MVPBuffer", mvpBuffer);
	bindProperties.emplace_back("LightBuffer", lightBuffer);
	bindProperties.emplace_back("baseTexture", textureView);

	for (auto i : m_frameRenderItems) {
		//Bind properties
		//DrawMesh
		//Use their own shader
		// frameRes.DrawMesh(
		// 	colorShader.get(),
		// 	psoManager.get(),
		// 	triangleMesh.get(),
		// 	colorFormat,
		// 	depthFormat,
		// 	bindProperties);

		//frameRes.DrawItem(renderItem, psoManager.get(), bindProperties);
	}
	frameRes.DrawMesh(
		colorShader.get(),
		psoManager.get(),
		triangleMesh.get(),
		colorFormat,
		depthFormat,
		bindProperties);
	// if (m_Meshes.find("E:\\Dev\\AwakeDXRenderEngine\\res\\bunny\\bunny.obj") != m_Meshes.end()) {
	// 	AddLog(log.get(), "HELLOHELLO???????");
	// 	frameRes.DrawMesh(
	// 		colorShader.get(),
	// 		psoManager.get(),
	// 		m_Meshes["E:\\Dev\\AwakeDXRenderEngine\\res\\bunny\\bunny.obj"],
	// 		colorFormat,
	// 		depthFormat,
	// 		bindProperties);
	// }

	//Imgui
	ID3D12DescriptorHeap* ppHeaps[] = {m_imgui_srvHeap->GetHeap()};
	cmdList->SetDescriptorHeaps(1, ppHeaps);
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	// Set necessary state.

	stateTracker.RestoreState(cmdList);
}
D3D12BetterSimpleBox::~D3D12BetterSimpleBox() {}
