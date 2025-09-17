#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <Core/DXDevice.h>

class TextureData {
public:

	/// <summary>
	/// 画像を読み込んでSRVを作成する
	/// </summary>
	TextureData(std::string filePath, DXDevice* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap);

	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureGPUHandle() const { return textureGpuHandle_; }
	ID3D12Resource* GetTextureResource() const { return textureResource_.Get(); }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermadiateResource_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureGpuHandle_;
	int width_ = 0;
	int height_ = 0;

	static int readTextureCount_;
};
