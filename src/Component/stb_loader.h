
#pragma once
#include <stb_image/stb_image.h>
#include "Resource/Texture.h"
#include <DXSampleHelper.h>
#include <Resource/UploadBuffer.h>
#include <DXRuntime/FrameResource.h>

Texture* Load2DTextureFromFile1(Device*, ID3D12GraphicsCommandList*, const char*, FrameResource*);
Texture* LoadCubeTextureFromFile(Device*, ID3D12GraphicsCommandList*, const char*, UploadBuffer* tmpUpload);
Texture* LoadHDRCubeTextureFromFile(Device*, ID3D12GraphicsCommandList*, const char*, UploadBuffer* tmpUpload);
Texture* Load2DBitFromFile(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, FrameResource* frameRes);