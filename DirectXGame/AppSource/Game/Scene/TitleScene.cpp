#include "TitleScene.h"
#include <imgui/ImGuizmo.h>

namespace {
	MatrixData* mat = nullptr;
	Material* material = nullptr;
	DirectionalLightData* dirLight = nullptr;
}

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	//KeyConfigの設定
	res_ = std::make_unique<ResourceGenerator>();
	res_->Initialize();
	res_->SetVertex(Primitive::Cube);

	res_->AddCBV(mat, ShaderType::VERTEX_SHADER);
	res_->AddCBV(material, ShaderType::PIXEL_SHADER);
	res_->AddCBV(dirLight, ShaderType::PIXEL_SHADER);
	res_->AddUAV(ShaderType::PIXEL_SHADER);
	res_->Create();

	mat->world = Matrix::MakeIdentity4x4();
	material->color = { 1.0f, 1.0f, 1.0f, 1.0f };
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	mat->wvp = camera_->GetVPMatrix();

	return nullptr;
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	render_->Draw(res_.get());
}
