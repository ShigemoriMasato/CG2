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

	//Gridの生成(軽量化のテストのためstd::listになってるけどそんなんやる必要ない)
	for (int i = 0; i < 1; ++i) {
		auto grid = std::make_unique<GridMaker>(camera_.get());
		grid->Initialize();
		gridMaker_.push_back(std::move(grid));
	}

	//モデルの生成
	descHandle_ = modelManager_->LoadModel("DefaultDesc");

	//モデルリソースの作成
	descModel_ = std::make_unique<ModelResource>();
	descModel_->Initialize(modelManager_->GetModelData(descHandle_));
	descModel_->SetCamera(camera_.get());

	//ギズモ用のデータ作成
	descTransform_ = {};
	descTransform_.scale = { 1.0f, 1.0f, 1.0f };
	descModel_->psoConfig_.isSwapChain = true;
	
	//ギズモに登録
	imguiWrapper_->AddItem("Desc", &worldMatrix_, &descTransform_);

	//Emitterの初期化
	testEmitter_ = std::make_unique<DefaultEmitter>(1000);
	testEmitter_->Initialize(camera_.get());

	//ImGuiのギズモにカメラを設定
	imguiWrapper_->SetCamera(camera_.get());

	//音の読み込み
	wavHandle = audio_->Load("fanfare.wav");
	mp3Handle = audio_->Load("Clear.mp3");
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	//Grid
	for (auto& grid : gridMaker_) {
		grid->Update();
	}

	//カメラの行列計算
	camera_->MakeMatrix();

	//Emitterの更新
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
	imguiWrapper_->GuizmoUpdate();
	descModel_->SetMatrixData(descTransform_.scale, descTransform_.rotation, descTransform_.position);


	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	//Gridの描画
	for (auto& grid : gridMaker_) {
		grid->Draw(render_);
	}

	//モデルの描画
	render_->Draw(descModel_.get());

	//パーティクルの描画
	testEmitter_->Draw(render_);
}
