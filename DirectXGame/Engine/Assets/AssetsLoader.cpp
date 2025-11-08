#include "AssetsLoader.h"
#include <filesystem>

namespace fs = std::filesystem;

void AssetsLoader::Initialize(DXDevice* device, SRVManager* srvmanager, ID3D12GraphicsCommandList* cmdList) {
	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(device, cmdList, srvmanager);

	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialize();

	modelManager_ = std::make_unique<ModelManager>();
	modelManager_->Initialize(textureManager_.get(), device);
}

AssetsID AssetsLoader::Load(std::filesystem::path filePath) {

	//すでに読み込まれている場合はそのIDを返す
	auto it = loadedAssets_.find(filePath.string());
	if (it != loadedAssets_.end()) {
		return it->second;
	}

	AssetsID id;
	id.id = nextID_++;

	if (fs::is_directory(filePath)) {

		//モデル
		modelManager_->LoadModel(filePath.string(), id.id);
		assetTypeMap_[id] = AssetType::Model;

	} else if(filePath.extension() == ".png" || filePath.extension() == ".jpg" || filePath.extension() == ".jpeg") {

		//テクスチャ
		textureManager_->LoadTexture(filePath.string(), id.id);
		assetTypeMap_[id] = AssetType::Texture;

	} else if (filePath.extension() == ".mp3" || filePath.extension() == ".wav") {

		//オーディオ
		audioManager_->Load(filePath, id.id);
		assetTypeMap_[id] = AssetType::Audio;

	} else {
		assert(false && "Unsupported asset file extension");
		return Load("Assets/Texture/Error.png");
	}

	return id;
}

TextureData* AssetsLoader::GetTextureData(AssetsID id) {
	if(assetTypeMap_[id] == AssetType::Texture) {
		return textureManager_->GetTextureData(id.id);
	}
	return nullptr;
}

AudioData* AssetsLoader::GetAudioData(AssetsID id) {
	if (assetTypeMap_[id] == AssetType::Audio) {
		return audioManager_->GetAudioData(id.id);
	}
	return nullptr;
}

ModelData* AssetsLoader::GetModelData(AssetsID id) {
	if (assetTypeMap_[id] == AssetType::Model) {
		return modelManager_->GetModelData(id.id);
	}
	return nullptr;
}
