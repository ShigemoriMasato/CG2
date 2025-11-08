#include "BaseResource.h"

DXDevice* BaseResource::dxDevice_ = nullptr;
AssetsLoader* BaseResource::loader_ = nullptr;
OffScreenManager* BaseResource::offscreenManager_ = nullptr;
SRVManager* BaseResource::srvManager_ = nullptr;

BaseResource::BaseResource() {
	debugLogger_ = Logger::getLogger("BaseResource");
	debugLogger_->debug("Create BaseResource");
}

BaseResource::BaseResource(const std::string debugName) {
	debugLogger_ = Logger::getLogger("BaseResource");
	debugLogger_->info("Create BaseResource: {}", debugName);
}

BaseResource::~BaseResource() {
	debugLogger_->debug("Delete BaseResource");
}

void BaseResource::SetTextureHandle(AssetsID id, uint32_t index) {
	if(index < 0 || index >= textureHandle_.size()) {
		return;
	}

	auto textureData = loader_->GetTextureData(id);
	if (textureData) {
		textureHandle_[index] = textureData->GetTextureGPUHandle();
	}
}

void BaseResource::SetTextureHandle(ScreenID offScreenIndex, uint32_t textureIndex) {
	if (textureIndex < 0 || textureIndex >= textureHandle_.size()) {
		return;
	}
	auto offScreenData = offscreenManager_->GetOffScreenData(offScreenIndex);
	if (offScreenData) {
		textureHandle_[textureIndex] = offScreenData->GetTextureGPUHandle();
	}
}

void BaseResource::MakeIndex(uint32_t*& indPtr, uint32_t indexNum) {
	indexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(uint32_t) * indexNum));
	indexResource->Map(0, nullptr, (void**)&indPtr);

	indexNum_ = indexNum;

	ibv_.BufferLocation = indexResource->GetGPUVirtualAddress();
	ibv_.SizeInBytes = sizeof(uint32_t) * indexNum;
	ibv_.Format = DXGI_FORMAT_R32_UINT;
}

void BaseResource::MakeUAV(uint32_t textureNum) {
	size_t beforeSize = textureHandle_.size();
	textureHandle_.resize(beforeSize + textureNum);
	auto white1x1 = loader_->Load("Assets/Texture/white1x1.png");
	for(size_t i = beforeSize; i < textureHandle_.size(); i++) {
		textureHandle_[int(i)] = loader_->GetTextureData(white1x1)->GetTextureGPUHandle();
	}
}
