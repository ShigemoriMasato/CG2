#include "InitializeScene.h"
#include <Function/MyString.h>
#include <Game/Scene/TitleScene.h>

void InitializeScene::Initialize() {
	LoadAllResources();

	auto keyManager = commonData_->keyManager_.get();

	//KeyConfigの設定

}

std::unique_ptr<BaseScene> InitializeScene::Update() {
	return std::make_unique<TitleScene>();
}

void InitializeScene::Draw() {
}

void InitializeScene::LoadAllResources() {

}
