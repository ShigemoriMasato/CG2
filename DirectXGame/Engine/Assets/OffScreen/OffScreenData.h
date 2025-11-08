#pragma once
#include <Core/DXDevice.h>
#include <Core/SRVManager.h>

class OffScreenData {
public:

	/// <summary>
	/// OffScreen用にリソースを作成する
	/// </summary>
	OffScreenData(int width, int height, float* clearColor, DXDevice* device, SRVManager* srvManager, ID3D12DescriptorHeap* rtv);
	~OffScreenData() = default;

	void EditBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter,
		D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	void DrawReady(ID3D12GraphicsCommandList* commandlist, bool isClear);

	ID3D12Resource* GetResource() const { return textureResource_.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureGPUHandle() const { return srvHandle_.GPU; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return rtvHandle_; }

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	SRVHandle srvHandle_;
	float clearColor_[4] = { 0.0f,0.0f,0.0f,1.0f };
	int width_ = 0;
	int height_ = 0;

	static int offScreenTextureCount_;

	//RTVHandle
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = {};

	//Depth
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	//バリアの状態
	D3D12_RESOURCE_STATES resourceState_ = D3D12_RESOURCE_STATE_COMMON;
};

