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

	camera2D_ = std::make_unique<Camera>();
	camera2D_->SetProjectionMatrix(OrthographicDesc());
	camera2D_->MakeMatrix();

	texture_ = std::make_unique<DrawResource>();
	textureHandle_ = textureManager_->LoadTexture("Assets/Texture/uvChecker.png");
	texture_->Initialize(4,6);
	texture_->textureHandle_ = textureHandle_;
	texture_->localPos_ = {
		{0.0f,  0.0f, 1.0f},
		{ 512.0f,  0.0f, 1.0f},
		{0.0f, 512.0f, 1.0f},
		{ 512.0f, 512.0f, 1.0f},
	};
	texture_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};
	texture_->normal_ = {
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
	};
	texture_->index_ = {
		0,1,2,
		1,3,2,
	};
	texture_->psoConfig_.isSwapChain = true;
	texture_->camera_ = camera2D_.get();
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	keys_ = commonData->keyManager->GetKeyStates();

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

	ImGui::Begin("Texture");
	ImGui::DragFloat2("TexPos", &texture_->texturePos_.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat2("TexScale", &texture_->textureScale_.x, 0.01f, 0.0f, 2.0f);
	ImGui::DragFloat("TexRotate", &texture_->textureRotate_, 0.5f, -180.0f, 180.0f);
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

	render_->Draw(texture_.get());

	testEmitter_->Draw(render_);
}
