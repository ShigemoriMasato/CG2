#include "InitializeScene.h"
#include <Func/MyString.h>
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

	auto audioFiles = SerchFilePathsAddChild("Assets/Audio");

	auto modelFolder = SearchFileNames("Assets/Model");

}
