#include "SceneManager.h"
#include <Core/EngineTerminal.h>
#include "Data/BaseScene.h"

SceneManager::SceneManager(std::unique_ptr<BaseScene> firstScene, EngineTerminal* engine) {
	engineTerminal_ = engine;

	commonData_ = std::make_shared<CommonData>();
	currentScene_ = std::move(firstScene);
	currentScene_->MoveScene(engineTerminal_, commonData_.get());
	currentScene_->Initialize();
}

SceneManager::~SceneManager() {
}

void SceneManager::Update() {

	if (nextScene_) {
		nextScene_->MoveScene(engineTerminal_, commonData_.get());
		currentScene_ = std::move(nextScene_);
		currentScene_->Initialize();
		nextScene_ = nullptr;
	}

	commonData_->keyManager_->Update();
	nextScene_ = currentScene_->Update();
}

void SceneManager::Draw() {
	currentScene_->Draw();
}
