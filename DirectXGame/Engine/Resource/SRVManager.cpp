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
	for(uint32_t i = 0; i < maxCount; ++i) {
		if(cpuMap_[i] == res) {
			return GetCPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
		}
	}

	Logger().Log("SRVManager::GetCPUHandle: Resource not found");
	throw std::runtime_error("SRVManager::GetGPUHandle: Resource not found");
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle(void* res) {
	for(uint32_t i = 0; i < maxCount; ++i) {
		if(gpuMap_[i] == res) {
			return GetGPUDesscriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, i);
		}
	}

	Logger().Log("SRVManager::GetGPUHandle: Resource not found");
	throw std::runtime_error("SRVManager::GetGPUHandle: Resource not found");
}
