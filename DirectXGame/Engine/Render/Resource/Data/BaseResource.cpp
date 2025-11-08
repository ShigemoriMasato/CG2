#include "BaseResource.h"

void BaseResource::SetTextureHandle(AssetsID id, uint32_t index) {
	if(index < 0 || index >= textureHandle_.size()) {
		return;
	}

	auto textureData = loader_->GetTextureData(id);
	if (textureData) {
		textureHandle_[index] = textureData->GetTextureGPUHandle();
	}
}

void BaseResource::MakeIndex(uint32_t*& indPtr, uint32_t indexNum) {
	indexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(uint32_t) * indexNum));
	indexResource->Map(0, nullptr, (void**)indPtr);

	D3D12_INDEX_BUFFER_VIEW ibv{};
	ibv.BufferLocation = indexResource->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * indexNum;
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv_.push_back(ibv);
}
