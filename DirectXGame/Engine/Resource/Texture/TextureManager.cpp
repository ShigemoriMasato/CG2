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
}

int TextureManager::LoadTexture(std::string filePath) {
	//すでに作成済みの場合はそのハンドルを返す
	if (textureHandleMap_.find(filePath) != textureHandleMap_.end()) {
		return textureHandleMap_[filePath];
	}
	
	textures_.emplace_back(std::make_unique<TextureData>(filePath, device_, commandList_, srvManager_));

	int handle = static_cast<int>(textures_.size() - 1);
	textureHandleMap_[filePath] = handle;
	return handle;
}

TextureData* TextureManager::GetTextureData(int handle) {
	if (handle < 0 || handle >= static_cast<int>(textures_.size())) {
		return nullptr;
	}
	return textures_[handle].get();
}
