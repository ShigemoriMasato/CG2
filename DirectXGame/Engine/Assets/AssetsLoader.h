#pragma once
#include <unordered_map>
#include <Core/ECS/EntityID.h>
#include "Texture/TextureManager.h"
#include "Audio/AudioManager.h"
#include "Model/ModelManager.h"

class AssetsLoader;

struct AssetsID : public EntityID {
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
		Unknown,
		Texture,
		Audio,
		Model,
		MaxCount
	};

	std::map<AssetsID, AssetType> assetTypeMap_;
	std::unordered_map<std::string, AssetsID> loadedAssets_;

	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<ModelManager> modelManager_;

	uint32_t nextID_ = 0;
};
