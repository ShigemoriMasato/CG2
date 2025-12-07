#include "TitleScene.h"

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

	AssetsID waterPlaneModel = assetsLoader_->Load("water");
	waterPlane_ = std::make_unique<WaterPlaneResource>();
	waterPlane_->Initialize(waterPlaneModel);
	waterPlane_->camera_ = camera_.get();

	waterObj_ = std::make_unique<WaterObjResource>();
	waterObj_->Initialize(modelID);
	waterObj_->position = { 0.0f, 0.0f, 0.0f };
	waterObj_->camera_ = camera_.get();
	waterObj_->waterHeight = 0.0f;
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

#ifdef USE_IMGUI
	ImGui::Begin("Clear Color");
	ImGui::ColorEdit4("ClearColor", &clearColor_.x);
	ImGui::End();
	render_->SetClearColor(clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w);

	ImGui::Begin("Water Object");
	ImGui::ColorEdit4("Color", &waterObjColor_.x);
	ImGui::DragFloat3("Scale", &waterObj_->scale.x, 0.1f, 0.1f);
	ImGui::DragFloat3("Rotation", &waterObj_->rotation.x, 0.1f);
	ImGui::DragFloat3("Position", &waterObj_->position.x, 0.1f);
	ImGui::End();

	ImGui::Begin("Water Plane");
	ImGui::ColorEdit4("Plane Color", &waterPlaneColor_.x);
	ImGui::DragFloat("Height", &waterPlane_->position.y, 0.01f);
	ImGui::End();

	waterPlane_->DrawImGui();
#endif

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();
	//render_->Draw(model_.get());
	render_->Draw(particleRes_.get());
	render_->Draw(waterObj_.get());
	render_->Draw(waterPlane_.get());
}