#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>
#include <Transform/Vector.h>

OffScreenManager::OffScreenManager() {
}

OffScreenManager::~OffScreenManager() {
}

void OffScreenManager::Initialize(DXDevice* device, SRVManager* srvManager) {
	device_ = device;
	srvManager_ = srvManager;
	ID3D12DescriptorHeap* rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
	rtvDescriptorHeap_.Attach(rawHeap);

	for(int i = 0; i < maxOffScreenCount_; ++i) {
		Vector4 clearColor = { 0.0f,0.0f,0.0f,1.0f };
		CreateOffScreen(1280, 720, &clearColor.x);
	}
}

int OffScreenManager::CreateOffScreen(int width, int height, float* clearColor) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, clearColor, device_, srvManager_, rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(ScreenID index) {
	return offScreens_[int(index)].get();
}
