#include "ModelManager.h"
#include <cassert>
#include <Func/MyString.h>
#include <filesystem>

namespace fs = std::filesystem;

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
}

void ModelManager::Initialize(AssetsLoader* assetsLoader, DXDevice* device) {
	assetsLoader_ = assetsLoader;
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
	models_[id]->LoadModel(directoryPath, objfile[0], assetsLoader_, device_);
}

ModelData* ModelManager::GetModelData(uint32_t modelHandle) {
	if (modelHandle < 0) {
		return nullptr;
	}
	return models_[modelHandle].get();
}
