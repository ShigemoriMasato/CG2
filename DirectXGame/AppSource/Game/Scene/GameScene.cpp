#include "GameScene.h"

void GameScene::Initialize() {

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetDistance(80.0f);

	keyCoating_ = std::make_unique<KeyCoating>(commonData->keyManager_.get());

	tetris_ = std::make_unique<Tetris>();
	tetris_->Initialize(keyCoating_.get(), debugCamera_.get());

}

std::unique_ptr<BaseScene> GameScene::Update() {
	float deltaTime = fpsObserver_->GetDeltatime();

	keyCoating_->Update(deltaTime);

	debugCamera_->Update();

	tetris_->Update(deltaTime);

	return nullptr;
}

void GameScene::Draw() {
	render_->PreDraw();

	tetris_->Draw(render_);
}
