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

void BaseResource::MakeIndex(uint32_t*& indPtr, uint32_t indexNum) {
	Resource res;
	res.res.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(uint32_t) * indexNum));
	res.res->Map(0, nullptr, (void**)&indPtr);

	allResources_.push_back(res);

	indexNum_ = indexNum;

	ibv_.BufferLocation = res.res->GetGPUVirtualAddress();
	ibv_.SizeInBytes = sizeof(uint32_t) * indexNum;
	ibv_.Format = DXGI_FORMAT_R32_UINT;
}

void BaseResource::MakeUAV() {
	useTexture_ = true;
}
