#include "GameScene.h"

void GameScene::Initialize() {

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	debugCamera_->SetDistance(80.0f);

	tetris_ = std::make_unique<Tetris>();
	tetris_->Initialize(debugCamera_.get());
}

std::unique_ptr<BaseScene> GameScene::Update() {

	debugCamera_->Update();
	float deltaTime = fpsObserver_->GetDeltatime();

	tetris_->Update(deltaTime);

	return nullptr;
}

void GameScene::Draw() {
	render_->PreDraw();

	tetris_->Draw(render_);
}
