#include "BGManager.h"

void BGManager::Initialize(AssetsLoader* loader, Camera* camera, std::mt19937 rand) {

	bgRes_ = std::make_unique<DrawResource>();
	auto bgID = loader->Load("BG_Ginga.png");
	bgRes_->Initialize(ShapeType::Plane);
	bgRes_->scale_ *= 2.0f;
	bgRes_->position_.z = 1000.0f;
	bgRes_->textureIndex_ = bgID.id;

	auto planetID = loader->Load("circle.png");
	planets_ = std::make_unique<Planets>();
	planets_->Initialize(planetID, camera, rand);
}

void BGManager::Update(float deltaTime) {
	planets_->Update(deltaTime);
}

void BGManager::Draw(Render* render) {
	render->Draw(bgRes_.get());
	planets_->Draw(render);
}

void BGManager::DrawImGui() {
	planets_->DrawImGui();
}
