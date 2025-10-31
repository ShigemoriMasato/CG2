#include "OffScreenManager.h"
#include <Core/DXCommonFunction.h>

OffScreenManager::OffScreenManager() {
}

OffScreenManager::~OffScreenManager() {
}

void OffScreenManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager) {
	// OffScreenIndex::MaxCount は enum class 型なので、int にキャストして比較する
	if(maxOffScreenCount_ < static_cast<int>(OffScreenIndex::MaxCount)) {
		assert(0 && "OffScreen num is too large!");
		return;
	}

	device_ = device;
	commandList_ = commandList;
	srvManager_ = srvManager;
	ID3D12DescriptorHeap* rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxOffScreenCount_, false);
	rtvDescriptorHeap_.Attach(rawHeap);

	for(int i = 0; i < maxOffScreenCount_; ++i) {

		if (i == static_cast<int>(OffScreenIndex::Title)) {
			float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			CreateOffScreen(1280, 720, clearColor);
			continue;
		}

		if (i == static_cast<int>(OffScreenIndex::Select)) {
			float clearColor[4] = { 0.3f, 0.5f, 0.7f, 1.0f };
			CreateOffScreen(1280, 720, clearColor);
			continue;
		}

		if (i == int(OffScreenIndex::GameWindow)) {
			float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
			CreateOffScreen(1280, 720, clearColor);
			continue;
		}
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		CreateOffScreen(1280, 720, clearColor);
	}
}

int OffScreenManager::CreateOffScreen(int width, int height, float* clearColor) {
	offScreens_.emplace_back(std::make_unique<OffScreenData>(width, height, clearColor, device_, commandList_, srvManager_, rtvDescriptorHeap_.Get()));
	return static_cast<int>(offScreens_.size() - 1);
}

OffScreenData* OffScreenManager::GetOffScreenData(OffScreenIndex index) {
	return offScreens_[int(index)].get();
}
