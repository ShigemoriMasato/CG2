#include "SRVManager.h"
#include <Core/DXCommonFunction.h>

SRVManager::SRVManager(DXDevice* device, int num) : maxCount(num) {

	ID3D12DescriptorHeap* rawHeap = nullptr;
	rawHeap = CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxCount, true);
	srvDescriptorHeap.Attach(rawHeap);

	descriptorSizeSRV = device->GetDescriptorSizeSRV();
}

SRVManager::~SRVManager() {
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUHandle() {
	if (readCountCPU_ >= maxCount) {
		readCountCPU_ -= maxCount / 2;
	}
	return GetCPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, readCountCPU_++);
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle() {
	if (readCountGPU_ >= maxCount) {
		readCountGPU_ -= maxCount / 2;
	}
	return GetGPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, readCountGPU_++);
}
