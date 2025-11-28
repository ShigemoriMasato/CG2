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


	bgRes_ = std::make_unique<DrawResource>();
	auto bgID = assetsLoader_->Load("BG_Ginga.png");
	bgRes_->Initialize(ShapeType::Plane);
	bgRes_->scale_ *= 2.0f;
	bgRes_->position_.z = 1000.0f;
	bgRes_->textureIndex_ = bgID.id;
	
	std::random_device rd;
	mt_ = std::mt19937(rd());

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
	rotateBlockEffect_->DrawImGui();

	return nullptr;
}

void GameScene::Draw() {
	render_->PreDraw();

	render_->Draw(bgRes_.get());

	tetris_->Draw(render_);

	rotateBlockEffect_->Draw(render_);

	if (gameOver_) {
		render_->Draw(gameOverRes_.get());
	}
}
