#pragma once
#include <unordered_map>
#include "Texture/TextureManager.h"
#include "Audio/AudioManager.h"
#include "Model/ModelManager.h"
#include "OffScreen/OffScreenManager.h"

class AssetsLoader;

struct AssetsID {
public:
	uint32_t get() { return id; }
private:
	friend class AssetsLoader;
	uint32_t id;
};

class AssetsLoader {
public:

	void Initialize(DXDevice* device, SRVManager* srvmanager, ID3D12GraphicsCommandList* cmdList);

	AssetsID Load(std::filesystem::path filePath);

	TextureData* GetTextureData(AssetsID id);
	AudioData* GetAudioData(AssetsID id);
	ModelData* GetModelData(AssetsID id);

private:

	enum class AssetType {
		Texture,
		Audio,
		Model
	};

	std::map<AssetsID, AssetType> assetTypeMap_;
	std::unordered_map<std::string, AssetsID> loadedAssets_;

	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<ModelManager> modelManager_;
	std::unique_ptr<OffScreenManager> offscreenManager_;

	uint32_t nextID_ = 0;
};
