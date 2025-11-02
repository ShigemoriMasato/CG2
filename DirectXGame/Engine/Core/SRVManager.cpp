#include "SRVManager.h"
#include <Core/DXCommonFunction.h>

SRVManager::SRVManager(DXDevice* device, int num) : maxCount(num) {

	ID3D12DescriptorHeap* rawHeap = nullptr;
	rawHeap = CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxCount, true);
	srvDescriptorHeap.Attach(rawHeap);

	descriptorSizeSRV = device->GetDescriptorSizeSRV();

	cpuMap_.resize(maxCount, nullptr);
	gpuMap_.resize(maxCount, nullptr);
}

SRVManager::~SRVManager() {
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUHandle(void* res) {
	while (true) {
		for (uint32_t i = cpuReadCount_; i < maxCount; ++i) {
			if (!cpuMap_[i]) {
				cpuMap_[i] = res;
				i = cpuReadCount_;
				return GetCPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
			}
		}
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle(void* res) {
	while (true) {
		for (uint32_t i = gpuReadCount_; i < maxCount; ++i) {
			if (!gpuMap_[i]) {
				gpuMap_[i] = res;
				i = gpuReadCount_;
				return GetGPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
			}
		}
	}
}
