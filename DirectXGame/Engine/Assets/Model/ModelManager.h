#pragma once
#include "ModelData.h"
#include <map>

class ModelManager {
public:

	ModelManager();
	~ModelManager();

	void Initialize(AssetsLoader* textureManager, DXDevice* device);

	void LoadModel(const std::string& directoryPath, uint32_t id);

	ModelData* GetModelData(uint32_t modelHandle);

private:

	std::map<uint32_t, std::unique_ptr<ModelData>> models_;

	AssetsLoader* assetsLoader_ = nullptr;
	DXDevice* device_ = nullptr;
};
