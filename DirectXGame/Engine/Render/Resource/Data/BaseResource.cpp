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

void BaseResource::SetModelData(AssetsID modelID) {
	ModelData* modelData = loader_->GetModelData(modelID);
	assert(modelData);
	auto vertexResource = modelData->GetVertexResource();
	vbv_.clear();
	vbv_.push_back(vertexResource.bufferView);
	vertexNum_ = vertexResource.vertexNum;

	auto indexResource = modelData->GetIndexResource();
	ibv_ = indexResource.bufferView;
	indexNum_ = indexResource.indexNum;

	psoConfig_.inputLayoutID = InputLayoutID::Model;
	psoConfig_.ps = "Model.PS.hlsl";
	psoConfig_.vs = "Model.VS.hlsl";
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
