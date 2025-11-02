#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>

class SRVManager;

class SRVHandle final {
public:
	SRVHandle(SRVManager* manager);
	~SRVHandle();

	const D3D12_CPU_DESCRIPTOR_HANDLE CPU;
	const D3D12_GPU_DESCRIPTOR_HANDLE GPU;

private:

	SRVManager* manager_;
	const int offset_;
};

class SRVManager {
public:

	SRVManager(DXDevice* device, int num);
	~SRVManager();

	ID3D12DescriptorHeap* GetHeap() { return srvDescriptorHeap.Get(); }

	std::unique_ptr<SRVHandle> GetHandle(bool unBreakable = false);

	uint32_t GetNextOffset();

private:

	friend class SRVHandle;

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	std::vector<bool> isUsed_;

	const uint32_t descriptorSizeSRV = 0;
	const uint32_t maxCount;

};
