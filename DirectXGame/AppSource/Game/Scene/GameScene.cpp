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

	std::random_device rd;
	mt_ = std::mt19937(rd());

	bgManager_ = std::make_unique<BGManager>();
	bgManager_->Initialize(assetsLoader_, debugCamera_.get(), mt_);

	rotateBlockEffect_ = std::make_unique<RotateBlockEffect>();
	rotateBlockEffect_->Initialize(16, debugCamera_.get(), mt_);
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
	
	int deletedLine = tetris_->IsLineDeleted();
	if (deletedLine) {
		rotateBlockEffect_->Rotation(static_cast<uint32_t>(deletedLine));
	}

	if (keyCoating_->GetKeyStates().at(Key::Restart)) {
		return std::make_unique<GameScene>();
	}

	rotateBlockEffect_->Update(deltaTime);
	bgManager_->Update(deltaTime);

#ifdef USE_IMGUI
	bgManager_->DrawImGui();
	rotateBlockEffect_->DrawImGui();
#endif

	return nullptr;
}

void GameScene::Draw() {
	render_->PreDraw();

	bgManager_->Draw(render_);

	tetris_->Draw(render_);

	rotateBlockEffect_->Draw(render_);

	if (gameOver_) {
		render_->Draw(gameOverRes_.get());
	}
}
