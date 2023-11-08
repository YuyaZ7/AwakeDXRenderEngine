
#pragma once
#include <stb_image/stb_image.h>
#include "Resource/Texture.h"
#include <DXSampleHelper.h>
#include <Resource/UploadBuffer.h>

Texture* Load2DTextureFromFile(Device*, ID3D12GraphicsCommandList*, const char*, UploadBuffer* tmpUpload);
Texture* LoadCubeTextureFromFile(Device*, ID3D12GraphicsCommandList*, const char*, UploadBuffer* tmpUpload);
Texture* LoadHDRCubeTextureFromFile(Device*, ID3D12GraphicsCommandList*, const char*, UploadBuffer* tmpUpload);
