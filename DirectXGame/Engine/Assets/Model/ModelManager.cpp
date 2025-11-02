#include "ModelManager.h"
#include <Function/MyString.h>
#include <filesystem>

namespace fs = std::filesystem;

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
}

void ModelManager::Initialize(TextureManager* textureManager, DXDevice* device) {
	textureManager_ = textureManager;
	device_ = device;

	models_.clear();
}

void ModelManager::LoadModel(const std::string& directoryPath, uint32_t id) {
	auto objfile = SearchFiles(directoryPath, ".obj");

	//objが見つからなかったら
	if (objfile.size() != 1) {

		auto glbfile = SearchFiles(directoryPath, ".glb");

		//glbも見つからなかったら
		if (glbfile.size() != 1) {

			auto gltffile = SearchFiles(directoryPath, ".gltf");

			//gltfも見つからなかったら
			if (gltffile.size() != 1) {
				assert(false && "Can't find .obj or glb file");
				return;
			}

			//gltfが見つかったら
			glbfile = gltffile;
		}

		//glbが見つかったら
		objfile = glbfile;

	}

	//モデルの読み込み
	models_[id] = std::make_unique<ModelData>();
	models_[id]->LoadModel(directoryPath, objfile[0], textureManager_, device_);
}

ModelData* ModelManager::GetModelData(uint32_t modelHandle) {
	if (modelHandle < 0 || modelHandle >= static_cast<int>(models_.size())) {
		return nullptr;
	}
	return models_[modelHandle].get();
}
