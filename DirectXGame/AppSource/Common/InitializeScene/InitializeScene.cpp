#include "InitializeScene.h"
#include <Function/MyString.h>
#include <Game/Scene/TitleScene.h>
#include <Render/Resource/ResourceGenerator.h>

void InitializeScene::Initialize() {
	LoadAllResources();

	auto keyManager = commonData_->keyManager_.get();

}

std::unique_ptr<BaseScene> InitializeScene::Update() {
	return std::make_unique<TitleScene>();
}

void InitializeScene::Draw() {
}

void InitializeScene::LoadAllResources() {

}
