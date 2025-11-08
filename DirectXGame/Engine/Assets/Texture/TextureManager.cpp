#include "TextureManager.h"
#include <Core/DXCommonFunction.h>

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	textures_.clear();
}

void TextureManager::Initialize(DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager) {
	device_ = device;
	commandList_ = commandList;
	srvManager_ = srvManager;

	textures_.clear();
}

void TextureManager::LoadTexture(std::string filePath, uint32_t id) {
	textures_[id] = std::make_unique<TextureData>(filePath, device_, commandList_, srvManager_);
}

TextureData* TextureManager::GetTextureData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(textures_.size())) {
		return nullptr;
	}
	return textures_[handle].get();
}
