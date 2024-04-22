#include "stb_loader.h"

// Texture* Load2DTextureFromFile(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, FrameResource* frameRes) {
// 	Texture* result = nullptr;
// 	int width, height, channels;
// 	unsigned char* idata = stbi_load(filepath, &width, &height, &channels, 0);

// 	if (idata == NULL) {
// 		printf("error loading the image/n");
// 		exit(1);
// 	}
// 	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
// 	uint pixelSize = 4;
// 	uint byteSize = width * height * pixelSize;
// 	unsigned char* srcData;

// 	if (channels == 3) {
// 		srcData = new unsigned char[width * height * pixelSize];
// 		for (int i = 0; i < width * height; ++i) {
// 			srcData[i * 4] = idata[i * 3];
// 			srcData[i * 4 + 1] = idata[i * 3 + 1];
// 			srcData[i * 4 + 2] = idata[i * 3 + 2];
// 			srcData[i * 4 + 3] = 1;
// 		}
// 	} else if (channels == 4) {
// 		srcData = idata;
// 	}
// 	result = new Texture(device, width, height, format, TextureDimension::Tex2D, 1, 1, Texture::TextureUsage::GenericColor, D3D12_RESOURCE_STATE_COPY_DEST);
// 	UploadBuffer* tmpUB = new UploadBuffer(device, byteSize);
// 	tmpUB->CopyData(0, {srcData, byteSize});
// 	stbi_image_free(srcData);
// 	srcData = nullptr;
// 	idata = nullptr;
// 	CD3DX12_TEXTURE_COPY_LOCATION dest(result->GetResource(), 0);
// 	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
// 	footprint.Offset = 0;
// 	footprint.Footprint.Width = width;
// 	footprint.Footprint.Height = height;
// 	footprint.Footprint.Depth = 1;
// 	footprint.Footprint.RowPitch = width * pixelSize;
// 	footprint.Footprint.Format = format;
// 	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
// 	srcLocation.pResource = tmpUB->GetResource();
// 	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
// 	srcLocation.PlacedFootprint = footprint;

// 	cmdList->CopyTextureRegion(&dest, 0, 0, 0, &srcLocation, nullptr);
// 	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
// 		result->GetResource(),
// 		D3D12_RESOURCE_STATE_COPY_DEST,
// 		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
// 	cmdList->ResourceBarrier(1, &barrier);
// 	frameRes->AddDelayDisposeResource(tmpUB->GetResource());
// 	return result;
// }
void Convert1BitTo8Bit(const unsigned char* source, unsigned char* target, int numPixels) {
	for (int i = 0; i < numPixels; ++i) {
		unsigned char bitValue = (source[i / 8] >> (i % 8)) & 1;

		target[i] = bitValue * 255;
	}
}
Texture* LoadHDRCubeTextureFromFile(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, UploadBuffer* tmpUpload) {
	Texture* result = nullptr;
	int width, height, channels;
	float* idata = stbi_loadf(filepath, &width, &height, &channels, 0);
	if (idata == NULL) {
		std::string reason = stbi_failure_reason();
		exit(1);
	}
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uint pixelSize = 16;
	uint byteSize = width * height * pixelSize;
	float* srcData;
	if (channels == 3) {
		srcData = new float[width * height * pixelSize];
		for (int i = 0; i < width * height; ++i) {
			srcData[i * 4] = static_cast<float>(idata[i * 3]);
			srcData[i * 4 + 1] = static_cast<float>(idata[i * 3 + 1]);
			srcData[i * 4 + 2] = static_cast<float>(idata[i * 3 + 2]);
			srcData[i * 4 + 3] = 1.0f;
		}
	} else if (channels == 4) {
		srcData = idata;
	}
	result = new Texture(device, width, height, format, TextureDimension::Cubemap, 1, 0, Texture::TextureUsage::GenericColor, D3D12_RESOURCE_STATE_COPY_DEST);
	tmpUpload = new UploadBuffer(device, byteSize);
	tmpUpload->CopyData(0, {reinterpret_cast<const vbyte*>(srcData), byteSize});
	stbi_image_free(srcData);
	srcData = nullptr;
	idata = nullptr;
	CD3DX12_TEXTURE_COPY_LOCATION dest(result->GetResource(), 0);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	footprint.Offset = 0;
	footprint.Footprint.Width = width;
	footprint.Footprint.Height = height;
	footprint.Footprint.Depth = 1;
	footprint.Footprint.RowPitch = width * pixelSize;
	footprint.Footprint.Format = format;
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = tmpUpload->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint = footprint;

	cmdList->CopyTextureRegion(&dest, 0, 0, 0, &srcLocation, nullptr);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		result->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
	return result;
}
Texture* LoadCubeTextureFromFile(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, UploadBuffer* tmpUpload) {
	Texture* result = nullptr;
	int width, height, channels;
	vbyte* idata = stbi_load(filepath, &width, &height, &channels, 0);
	if (idata == NULL) {
		std::string reason = stbi_failure_reason();
		exit(1);
	}
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint pixelSize = 4;
	uint byteSize = width * height * pixelSize;
	vbyte* srcData;
	if (channels == 3) {
		srcData = new vbyte[width * height * pixelSize];
		for (int i = 0; i < width * height; ++i) {
			srcData[i * 4] = idata[i * 3];
			srcData[i * 4 + 1] = idata[i * 3 + 1];
			srcData[i * 4 + 2] = idata[i * 3 + 2];
			srcData[i * 4 + 3] = 1;
		}
	} else if (channels == 4) {
		srcData = idata;
	}
	result = new Texture(device, width, height, format, TextureDimension::Cubemap, 1, 0, Texture::TextureUsage::GenericColor, D3D12_RESOURCE_STATE_COPY_DEST);
	tmpUpload = new UploadBuffer(device, byteSize);
	tmpUpload->CopyData(0, {srcData, byteSize});
	stbi_image_free(srcData);
	srcData = nullptr;
	idata = nullptr;
	CD3DX12_TEXTURE_COPY_LOCATION dest(result->GetResource(), 0);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	footprint.Offset = 0;
	footprint.Footprint.Width = width;
	footprint.Footprint.Height = height;
	footprint.Footprint.Depth = 1;
	footprint.Footprint.RowPitch = width * pixelSize;
	footprint.Footprint.Format = format;
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = tmpUpload->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint = footprint;

	cmdList->CopyTextureRegion(&dest, 0, 0, 0, &srcLocation, nullptr);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		result->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
	return result;
}

Texture* Load2DTextureFromFile1(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, FrameResource* frameRes) {
	Texture* result = nullptr;
	int width, height, channels;
	unsigned char* idata = stbi_load(filepath, &width, &height, &channels, 0);

	if (idata == NULL) {
		printf("error loading the image/n");
		exit(1);
	}
	DXGI_FORMAT format;
	uint pixelByteSize;
	unsigned char* srcData;
	if (channels == 1) {
		format = DXGI_FORMAT_R8_UNORM;
		pixelByteSize = 1;
		srcData = idata;
	} else if (channels == 3) {
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		pixelByteSize = 4;
		srcData = new unsigned char[width * height * pixelByteSize];
		for (int i = 0; i < width * height; ++i) {
			srcData[i * 4] = idata[i * 3];
			srcData[i * 4 + 1] = idata[i * 3 + 1];
			srcData[i * 4 + 2] = idata[i * 3 + 2];
			srcData[i * 4 + 3] = 1;
		}

	} else if (channels == 4) {
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		pixelByteSize = 4;
		srcData = idata;
	}
	uint byteSize = width * height * pixelByteSize;

	result = new Texture(device, width, height, format, TextureDimension::Tex2D, 1, 1, Texture::TextureUsage::GenericColor, D3D12_RESOURCE_STATE_COPY_DEST);
	UploadBuffer* tmpUB = new UploadBuffer(device, byteSize);
	tmpUB->CopyData(0, {srcData, byteSize});
	stbi_image_free(srcData);
	srcData = nullptr;
	idata = nullptr;
	CD3DX12_TEXTURE_COPY_LOCATION dest(result->GetResource(), 0);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	footprint.Offset = 0;
	footprint.Footprint.Width = width;
	footprint.Footprint.Height = height;
	footprint.Footprint.Depth = 1;
	footprint.Footprint.RowPitch = width * pixelByteSize;
	footprint.Footprint.Format = format;
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = tmpUB->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint = footprint;

	cmdList->CopyTextureRegion(&dest, 0, 0, 0, &srcLocation, nullptr);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		result->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
	frameRes->AddDelayDisposeResource(tmpUB->GetResource());
	return result;
}
Texture* Load2DBitFromFile(Device* device, ID3D12GraphicsCommandList* cmdList, const char* filepath, FrameResource* frameRes) {
	Texture* result = nullptr;
	int width, height, channels;
	unsigned char* idata = stbi_load(filepath, &width, &height, &channels, 0);
	if (idata == NULL) {
		printf("error loading the image/n");
		exit(1);
	}
	DXGI_FORMAT format = DXGI_FORMAT_R8_UNORM;
	uint byteSize = width * height;
	std::vector<unsigned char> srcData(byteSize);
	Convert1BitTo8Bit(idata, srcData.data(), byteSize);
	result = new Texture(device, width, height, format, TextureDimension::Tex2D, 1, 1, Texture::TextureUsage::GenericColor, D3D12_RESOURCE_STATE_COPY_DEST);
	UploadBuffer* tmpUB = new UploadBuffer(device, byteSize);
	tmpUB->CopyData(0, {srcData.data(), byteSize});
	stbi_image_free(idata);
	CD3DX12_TEXTURE_COPY_LOCATION dest(result->GetResource(), 0);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	footprint.Offset = 0;
	footprint.Footprint.Width = width;
	footprint.Footprint.Height = height;
	footprint.Footprint.Depth = 1;
	footprint.Footprint.RowPitch = width;
	footprint.Footprint.Format = format;
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = tmpUB->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint = footprint;

	cmdList->CopyTextureRegion(&dest, 0, 0, 0, &srcLocation, nullptr);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		result->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
	frameRes->AddDelayDisposeResource(tmpUB->GetResource());
	return result;
}