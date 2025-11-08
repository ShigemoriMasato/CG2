#include "TitleScene.h"
#include <imgui/ImGuizmo.h>

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	res_ = std::make_unique<DrawResource>();
	res_->Initialize(ShapeType::Cube);
	res_->camera_ = camera_.get();
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();
	render_->Draw(res_.get());
}
