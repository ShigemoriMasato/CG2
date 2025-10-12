#pragma once
#include "ModelData.h"

class ModelManager {
public:

	ModelManager(TextureManager* textureManager, DXDevice* device);
	~ModelManager();

	int LoadModel(const std::string& directoryPath);

	ModelData* GetModelData(int modelHandle);

private:

	std::unordered_map<std::string, int> modelHandleMap_;
	std::vector<std::unique_ptr<ModelData>> models_;

	const std::string basePath_ = "Assets/Model/";

	TextureManager* textureManager_ = nullptr;
	DXDevice* device_ = nullptr;
};

