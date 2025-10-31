#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>

class SRVManager {
public:

	SRVManager(DXDevice* device, int num);
	~SRVManager();

	ID3D12DescriptorHeap* GetHeap() { return srvDescriptorHeap.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

private:

	//SRVHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	uint32_t descriptorSizeSRV = 0;
	uint32_t readCountCPU_ = 0;
	uint32_t readCountGPU_ = 0;

	const uint32_t maxCount;

};
