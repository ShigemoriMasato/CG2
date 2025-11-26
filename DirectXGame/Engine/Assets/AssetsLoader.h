#pragma once
#include <unordered_map>
#include <Core/ECS/EntityID.h>
#include "Texture/TextureManager.h"
#include "Audio/AudioManager.h"
#include "Model/ModelManager.h"

struct AssetsID : public EntityID {
};

class AssetsLoader {
public:

	AssetsLoader() = default;
	~AssetsLoader() = default;

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
		VerS1_PixC1_Tex,
		MaxCount
	};

private:

	AssetType CheckAssetsType(std::filesystem::path filePath);

private:

	std::map<AssetsID, AssetType> assetTypeMap_;
	std::unordered_map<std::string, AssetsID> loadedAssets_;

	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<AudioManager> audioManager_;
	std::unique_ptr<ModelManager> modelManager_;

	uint32_t nextTextureID_ = 0;
	uint32_t nextOtherID_ = 1024;
};
