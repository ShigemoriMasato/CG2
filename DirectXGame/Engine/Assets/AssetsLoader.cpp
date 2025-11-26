#include "AssetsLoader.h"
#include "Model/ModelManager.h"
#include <filesystem>

namespace fs = std::filesystem;

void AssetsLoader::Initialize(DXDevice* device, SRVManager* srvmanager, ID3D12GraphicsCommandList* cmdList) {
	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(device, cmdList, srvmanager);

	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialize();

	modelManager_ = std::make_unique<ModelManager>();
	modelManager_->Initialize(this, device);

	Load("Assets/Texture/white1x1.png");
}

AssetsID AssetsLoader::Load(std::filesystem::path filePath) {
	//拡張子からタイプを確認
	AssetType type = CheckAssetsType(filePath);

	if (type == AssetType::Unknown) {
		assert(false && "Unsupported asset type");
		return AssetsID{ 0xffffffff };
	}

	//Assetsから書いてあるか確認。
	const std::string basePath = "Assets/";

	bool isWriteBasePath = true;

	//文字数が足りてなかったらBasePathを追加
	if (filePath.string().length() < basePath.length()) {

		isWriteBasePath = false;

	} else {

		for (int i = 0; i < int(basePath.length()); ++i) {
			if (filePath.string()[i] != basePath[i]) {
				isWriteBasePath = false;
				break;
			}
		}
	}

	//Assetsから書いてなかったら追加
	if (!isWriteBasePath) {
		switch (type) {
		case AssetType::Texture:
			filePath = "Assets/Texture/" + filePath.string();
			break;

		case AssetType::Audio:
			filePath = "Assets/Audio/" + filePath.string();
			break;

		case AssetType::VerS1_PixC1_Tex:
			filePath = "Assets/Model/" + filePath.string();
			break;
		}
	}

	//すでに読み込まれている場合はそのIDを返す
	auto it = loadedAssets_.find(filePath.string());
	if (it != loadedAssets_.end()) {
		return it->second;
	}

	AssetsID id;
	if (type == AssetType::Texture) {
		id.id = nextTextureID_++;
	} else {
		id.id = nextOtherID_++;
	}

	switch (type) {
	case AssetsLoader::AssetType::Texture:
		textureManager_->LoadTexture(filePath.string(), id.id);
		break;
	case AssetsLoader::AssetType::Audio:
		audioManager_->Load(filePath, id.id);
		break;
	case AssetsLoader::AssetType::VerS1_PixC1_Tex:
		modelManager_->LoadModel(filePath.string(), id.id);
		break;
	}

	loadedAssets_[filePath.string()] = id;
	assetTypeMap_[id] = type;

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
	if (assetTypeMap_[id] == AssetType::VerS1_PixC1_Tex) {
		return modelManager_->GetModelData(id.id);
	}
	return nullptr;
}

AssetsLoader::AssetType AssetsLoader::CheckAssetsType(std::filesystem::path filePath) {

	if (filePath.extension() == ".png" || filePath.extension() == ".jpg" || filePath.extension() == ".jpeg") {

		//テクスチャ
		return AssetType::Texture;

	} else if (filePath.extension() == ".mp3" || filePath.extension() == ".wav") {

		//オーディオ
		return AssetType::Audio;

	} else if (fs::is_directory(filePath) || fs::is_directory("Assets/Model/" + filePath.string())) {
		
		//モデル
		return AssetType::VerS1_PixC1_Tex;

	}

	return AssetType::Unknown;

}
