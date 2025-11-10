#include "TitleScene.h"
#include <imgui/ImGuizmo.h>

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	auto modelID = assetsLoader_->Load("DefaultDesc");

	model_ = std::make_unique<DrawResource>();
	model_->Initialize(modelID);
	model_->camera_ = camera_.get();

	particleRes_ = std::make_unique<ParticleResource>();
	particleRes_->Initialize(ShapeType::Plane, 10);
	particleRes_->camera_ = camera_.get();
	
	AssetsID white1x1 = assetsLoader_->Load("white1x1.png");
	AssetsID uvChecker = assetsLoader_->Load("uvChecker.png");
	AssetsID circle = assetsLoader_->Load("circle.png");

	std::vector<AssetsID> textures = {
		white1x1,
		uvChecker,
		circle
	};

	for (int i = 0; i < 10; ++i) {
		particleRes_->position_[i] = { static_cast<float>(i) - 3.0f, 2.0f, 0.0f };
		particleRes_->textureIndex_[i] = textures[i % textures.size()];
	}
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();
	render_->Draw(model_.get());
	render_->Draw(particleRes_.get());
}