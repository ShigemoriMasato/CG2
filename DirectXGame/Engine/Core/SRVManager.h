#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>

class SRVManager;

class SRVHandle final {
public:
	SRVHandle() = default;
	~SRVHandle();

	void UpdateHandle(SRVManager* manager, bool isTexture = false);

	D3D12_CPU_DESCRIPTOR_HANDLE CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE GPU;

private:

	SRVManager* manager_;
	int offset_;

	bool isUpdated_ = false;
};

class SRVManager {
public:

	SRVManager(DXDevice* device, int num);
	~SRVManager() = default;

	ID3D12DescriptorHeap* GetHeap() { return srvDescriptorHeap.Get(); }

	uint32_t GetNextOffset(bool isTexture);

private:

	friend class SRVHandle;

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	std::vector<bool> isUsed_;

	const uint32_t descriptorSizeSRV = 0;
	const uint32_t maxCount;

};
