#include "InitializeScene.h"
#include <Math/MyString.h>
#include <Game/Scene/TitleScene.h>

void InitializeScene::Initialize() {
	LoadAllResources();

	auto keyManager = commonData->keyManager_.get();

	//KeyConfigの設定

}

std::unique_ptr<BaseScene> InitializeScene::Update() {
	return std::make_unique<TitleScene>();
}

void InitializeScene::Draw() {
}

void InitializeScene::LoadAllResources() {

	auto textureFile = SerchFilePathsAddChild("Assets/Texture", ".png");

	for (const auto& filePath : textureFile) {
		std::string relativePath = "Assets/Texture/" + filePath;
		textureManager_->LoadTexture(relativePath);
	}

	auto audioFiles = SerchFilePathsAddChild("Assets/Audio");

	for (const auto& filePath : audioFiles) {
		std::string relativePath = filePath;
		audio_->Load(relativePath);
	}

	auto modelFilesObj = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesObj) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

	auto modelFilesGltf = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesGltf) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

	auto modelFilesGlb = SearchFileNames("Assets/Model");

	for (const auto& filePath : modelFilesGlb) {
		std::string relativePath = filePath;
		modelManager_->LoadModel(relativePath);
	}

}
