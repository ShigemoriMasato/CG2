#include "GameScene.h"

void GameScene::Initialize() {

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetDistance(80.0f);
	debugCamera_->Update();

	keyCoating_ = std::make_unique<KeyCoating>(commonData->keyManager_.get());

	tetris_ = std::make_unique<Tetris>();
	tetris_->Initialize(keyCoating_.get(), debugCamera_.get());

	gameOverRes_ = std::make_unique<DrawResource>();
	auto goID = assetsLoader_->Load("gameover");
	gameOverRes_->Initialize(goID);
	gameOverRes_->position_ = { 0.0f, 2.5f, -70.0f };
	gameOverRes_->camera_ = debugCamera_.get();
}

std::unique_ptr<BaseScene> GameScene::Update() {
	float deltaTime = fpsObserver_->GetDeltatime();

	keyCoating_->Update(deltaTime);

#ifndef SH_RELEASE
	debugCamera_->Update();
#endif

	gameOver_ = tetris_->IsGameOver();

	if (!gameOver_) {
		tetris_->Update(deltaTime);
	}

	if (keyCoating_->GetKeyStates().at(Key::Restart)) {
		return std::make_unique<GameScene>();
	}

	return nullptr;
}

void GameScene::Draw() {
	render_->PreDraw();

	tetris_->Draw(render_);

	if (gameOver_) {
		render_->Draw(gameOverRes_.get());
	}
}
