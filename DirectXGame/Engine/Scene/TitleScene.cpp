#include "TitleScene.h"
#include <imgui/ImGuizmo.h>

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	imguiWrapper_ = std::make_unique<ImGuiWrapper>();

	for (int i = 0; i < 1; ++i) {
		auto grid = std::make_unique<GridMaker>(camera_.get());
		grid->Initialize();
		gridMaker_.push_back(std::move(grid));
	}

	descHandle_ = modelManager_->LoadModel("DefaultDesc");
	descModel_ = std::make_unique<ModelResource>();
	descModel_->Initialize(modelManager_, descHandle_);
	descModel_->camera_ = camera_.get();
	descTransform_ = {};
	descTransform_.scale = { 1.0f, 1.0f, 1.0f };
	descModel_->psoConfig_.isSwapChain = true;
	imguiWrapper_->AddItem("Desc", &worldMatrix_, &descTransform_);

	testEmitter_ = std::make_unique<DefaultEmitter>(1000);
	testEmitter_->Initialize(camera_.get());

	imguiWrapper_->SetCamera(camera_.get());

	wavHandle = audio_->Load("fanfare.wav");
	mp3Handle = audio_->Load("Clear.mp3");
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	keys_ = commonData->keyManager_->GetKeyStates();

	camera_->Update();

	for (auto& grid : gridMaker_) {
		grid->Update();
	}

	camera_->MakeMatrix();
	testEmitter_->Update();

	// =====================- Audioテスト -===========================
	ImGui::Begin("Audio");
	if (ImGui::Button("WAV")) {
		audio_->Play(wavHandle, false);
	}
	if (ImGui::Button("MP3")) {
		audio_->Play(mp3Handle, false);
	}
	ImGui::End();


	// ====================- GuIzmoテスト -============================
	//imguiWrapper_->GuizmoUpdate();

	// ====================- Input系のテスト -==========================
	if (keys_[Key::Left]) {
		descModel_->position_.x -= 0.03f;
	}

	if (keys_[Key::Right]) {
		descModel_->position_.x += 0.03f;
	}

	if (keys_[Key::Up]) {
		descModel_->position_.z += 0.03f;
	}

	if (keys_[Key::Down]) {
		descModel_->position_.z -= 0.03f;
	}

	int indexI = 0;
	ImGui::Begin("Key");
	for(const auto&[key, value] : keys_) {
		if(value) {
			ImGui::Text("Key %d is pressed", static_cast<int>(key));
		}
		++indexI;
	}
	ImGui::End();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	for(auto& grid : gridMaker_) {
		grid->Draw(render_);
	}

	render_->Draw(descModel_.get());

	testEmitter_->Draw(render_);
}
