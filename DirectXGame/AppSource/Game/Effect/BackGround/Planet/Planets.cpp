#include "Planets.h"

Planets::~Planets() {

	binaryManager_->RegistOutput(createInterval_);
	binaryManager_->RegistOutput(kAcceleration_);
	binaryManager_->RegistOutput(initPosMin_);
	binaryManager_->RegistOutput(initPosMax_);
	binaryManager_->RegistOutput(initPosZ_);
	binaryManager_->RegistOutput(scale_);
	binaryManager_->RegistOutput(deleteLine_);

	binaryManager_->Write("Planets.dat");

}

void Planets::Initialize(AssetsID textureIndex, Camera* camera, std::mt19937 rand) {
	rand_ = rand;
	planetParticles_ = std::make_unique<ParticleResource>();
	planetParticles_->Initialize(ShapeType::Plane, kPlanetNum_);

	planetParticles_->camera_ = camera;
	std::fill(planetParticles_->textureIndex_.begin(), planetParticles_->textureIndex_.end(), textureIndex);
	std::fill(planetParticles_->color_.begin(), planetParticles_->color_.end(), 0);

	active_.resize(kPlanetNum_, false);
	speed_.resize(kPlanetNum_, 0.0f);

	binaryManager_ = std::make_unique<BinaryManager>();
	auto data = binaryManager_->Read("Planets.dat");
	int i = 0;

	if(i >= data.size()) {
		return;
	}

	planetParticles_->psoConfig_.blendID = BlendStateID::Add;

	createInterval_ = BinaryManager::Reverse<float>(data[i++]);
	kAcceleration_ = BinaryManager::Reverse<float>(data[i++]);
	initPosMin_ = BinaryManager::Reverse<Vector2>(data[i++]);
	initPosMax_ = BinaryManager::Reverse<Vector2>(data[i++]);
	initPosZ_ = BinaryManager::Reverse<float>(data[i++]);
	scale_ = BinaryManager::Reverse<Vector3>(data[i++]);
	std::fill(planetParticles_->scale_.begin(), planetParticles_->scale_.end(), scale_);
	deleteLine_ = BinaryManager::Reverse<float>(data[i++]);

}

void Planets::Update(float deltaTime) {
	createTimer_ += deltaTime;
	while (createTimer_ >= createInterval_) {
		createTimer_ = createTimer_ - createInterval_;
		Create();
	}

	for (size_t i = 0; i < kPlanetNum_; ++i) {
		if(!active_[i]) {
			continue;
		}

		//todo 速度調整
		speed_[i] += kAcceleration_ * deltaTime;
		planetParticles_->position_[i].z += speed_[i] * deltaTime;

		//todo 変数化
		if (planetParticles_->position_[i].z > deleteLine_) {
			active_[i] = false;
			speed_[i] = 0.0f;
			planetParticles_->color_[i] = 0;
		}
	}
}

void Planets::Draw(Render* render) {
	render->Draw(planetParticles_.get());
}

void Planets::Create() {
	int i;
	for(i = 0; i < kPlanetNum_; ++i) {
		if (!active_[i]) {
			break;
		}
	}

	if (i >= kPlanetNum_) {
		return;
	}

	active_[i] = true;
	//位置決定
	std::uniform_real_distribution<float> distX(initPosMin_.x, initPosMax_.x);
	std::uniform_real_distribution<float> distY(initPosMin_.y, initPosMax_.y);
	planetParticles_->position_[i] = { distX(rand_), distY(rand_), initPosZ_ };
	planetParticles_->color_[i] = 0xffffffff;
}

void Planets::DrawImGui() {
	ImGui::Begin("Planets Effect");

	int activeCount = 0;
	for (size_t i = 0; i < kPlanetNum_; ++i) {
		if (active_[i]) {
			++activeCount;
		}
	}
	ImGui::Text("%d / %d", activeCount, kPlanetNum_);

	ImGui::DragFloat("Create Interval", &createInterval_, 0.001f, 0.0f);
	ImGui::DragFloat("Acceleration", &kAcceleration_, 0.0f, 0.0f);

	ImGui::DragFloat2("Init Pos Min", &initPosMin_.x, 0.1f, -100.0f, 100.0f);
	initPosMin_ = Vector2(std::min(initPosMin_.x, initPosMax_.x), std::min(initPosMin_.y, initPosMax_.y));
	ImGui::DragFloat2("Init Pos Max", &initPosMax_.x, 0.1f, -100.0f, 100.0f);
	initPosMax_ = Vector2(std::max(initPosMin_.x, initPosMax_.x), std::max(initPosMin_.y, initPosMax_.y));
	ImGui::DragFloat("Init Pos Z", &initPosZ_, 0.1f);

	ImGui::DragFloat("Scale", &scale_.x, 0.1f, 0.1f, 100.0f);
	scale_.y = scale_.x;
	scale_.z = scale_.x;
	std::fill(planetParticles_->scale_.begin(), planetParticles_->scale_.end(), scale_);

	ImGui::SliderFloat("Delete Line", &deleteLine_, 100.0f, 1000.0f);

	ImGui::End();
}
