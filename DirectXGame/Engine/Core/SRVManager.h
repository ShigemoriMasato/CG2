#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>

class SRVManager {
public:

	SRVManager(DXDevice* device, int num);
	~SRVManager();

	ID3D12DescriptorHeap* GetHeap() { return srvDescriptorHeap.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(void* res);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(void* res);

private:

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	uint32_t descriptorSizeSRV = 0;

	std::vector<void*> cpuMap_;
	uint32_t cpuReadCount_ = 0;
	std::vector<void*> gpuMap_;
	uint32_t gpuReadCount_ = 0;

	const uint32_t maxCount;

};
