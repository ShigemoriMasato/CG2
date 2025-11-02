#include "SceneManager.h"
#include <Core/EngineTerminal.h>
#include "Data/BaseScene.h"

SceneManager::SceneManager(std::unique_ptr<BaseScene> firstScene, EngineTerminal* engine) {
	tools_ = SceneTools{
		engine->GetModelManager(),
		engine->GetTextureManager(),
		engine->GetRender(),
		engine->GetInput(),
		engine->GetAudioManager(),
		engine->GetOffScreenManager(),
		engine->GetFPSObserver()
	};

	commonData_ = std::make_shared<CommonData>();
	nextScene_ = std::move(firstScene);
}

SceneManager::SceneManager(std::unique_ptr<BaseScene> firstScene, BaseScene* createdScene) {
	tools_ = createdScene->CreateEngineTools();

	commonData_ = std::make_shared<CommonData>();
	nextScene_ = std::move(firstScene);
}

SceneManager::~SceneManager() {
}

void SceneManager::SetCommonData(CommonData* commonData) {
	*commonData_ = *commonData;
}

void SceneManager::Update() {

	if (nextScene_) {
		nextScene_->MoveScene(tools_, commonData_.get());
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
