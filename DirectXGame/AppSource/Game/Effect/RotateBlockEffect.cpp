#include "RotateBlockEffect.h"

RotateBlockEffect::~RotateBlockEffect() {

	binaryManager_->RegistOutput(kBlockNum_);


	//Block -=======================================

	for (int i = 0; i < kBlockNum_; ++i) {
		binaryManager_->RegistOutput(effectBlock_->scale_[i]);
		binaryManager_->RegistOutput(effectBlock_->rotate_[i]);
		binaryManager_->RegistOutput(effectBlock_->position_[i]);
		binaryManager_->RegistOutput(effectBlock_->color_[i]);
		binaryManager_->RegistOutput(effectBlock_->outlineColor_[i]);
	}

	binaryManager_->RegistOutput(kRotateSpeed_);
	binaryManager_->RegistOutput(kRotateAttenuation_);

	//PointLight -=======================================

	binaryManager_->RegistOutput(kLightLife_);
	binaryManager_->RegistOutput(kSpeed_);
	binaryManager_->RegistOutput(kLightIntensity_);
	binaryManager_->RegistOutput(spawnInterval_);
	binaryManager_->RegistOutput(initPosMin_);
	binaryManager_->RegistOutput(initPosMax_);

	//DirectionalLight -=======================================

	for (int i = 0; i < 3; ++i) {
		binaryManager_->RegistOutput(directionalLightData_[i].intensity);
		binaryManager_->RegistOutput(directionalLightData_[i].direction);
		binaryManager_->RegistOutput(directionalLightData_[i].color);
	}

	binaryManager_->Write(kBinaryFileName_);
}

void RotateBlockEffect::Initialize(uint32_t instance, Camera* camera, std::mt19937 rand) {
	rand_ = rand;
	kBlockNum_ = instance;

	effectBlock_ = std::make_unique<BlockResource>();
	effectBlock_->Initialize(instance);
	effectBlock_->camera_ = camera;

	effectBlock_->MakeDirectionalAndPointLight(3, kPointLightNum_);

	for (int i = 0; i < kPointLightNum_; ++i) {
		effectBlock_->pointLightData_[i].intensity = 0.0f;
	}

	binaryManager_ = std::make_unique<BinaryManager>();
	auto data = binaryManager_->Read(kBinaryFileName_);

	lightObj_ = std::make_unique<ParticleResource>();
	lightObj_->Initialize(ShapeType::Sphere, kPointLightNum_);
	lightObj_->camera_ = camera;
	lightObj_->psoConfig_.blendID = BlendStateID::Add;

	lightActive_.resize(kPointLightNum_, false);
	lightLife_.resize(kPointLightNum_, 0.0f);
	directionalLightData_.resize(3);

	if (!data.empty()) {
		int size = BinaryManager::Reverse<int>(data[0]);

		int i = 1;
		for (int index = 0; index < size && index < int(instance); index++) {
			effectBlock_->scale_[index] = BinaryManager::Reverse<Vector3>(data[i++]);
			effectBlock_->rotate_[index] = BinaryManager::Reverse<Vector3>(data[i++]);
			effectBlock_->position_[index] = BinaryManager::Reverse<Vector3>(data[i++]);
			effectBlock_->color_[index] = BinaryManager::Reverse<uint32_t>(data[i++]);
			effectBlock_->outlineColor_[index] = BinaryManager::Reverse<uint32_t>(data[i++]);
		}

		colorBuffer_ = ConvertColor(effectBlock_->color_[0]);
		outlineColorBuffer_ = ConvertColor(effectBlock_->outlineColor_[0]);

		if (i >= data.size()) {
			return;
		}

		rotateSpeed_ = BinaryManager::Reverse<Vector3>(data[i++]);
		kRotateAttenuation_ = BinaryManager::Reverse<float>(data[i++]);

		if (i >= data.size()) {
			return;
		}

		kLightLife_ = BinaryManager::Reverse<float>(data[i++]);
		kSpeed_ = BinaryManager::Reverse<float>(data[i++]);
		kLightIntensity_ = BinaryManager::Reverse<float>(data[i++]);
		spawnInterval_ = BinaryManager::Reverse<float>(data[i++]);
		initPosMin_ = BinaryManager::Reverse<Vector3>(data[i++]);
		initPosMax_ = BinaryManager::Reverse<Vector3>(data[i++]);

		if(i >= data.size()) {
			return;
		}

		for(int index = 0; index < 3; ++index) {
			directionalLightData_[index].intensity = BinaryManager::Reverse<float>(data[i++]);
			directionalLightData_[index].direction = BinaryManager::Reverse<Vector3>(data[i++]);
			directionalLightData_[index].color = BinaryManager::Reverse<Vector4>(data[i++]);
		}

	}

}

void RotateBlockEffect::Update(float deltaTime) {

	spawnTimer_ += deltaTime;
	
	//ライトのスポーン処理
	if (spawnTimer_ >= spawnInterval_) {
		spawnTimer_ = std::clamp(spawnTimer_ - spawnInterval_, 0.0f, spawnInterval_);

		int index = 0;
		for (index; index < kPointLightNum_; ++index) {
			if (!lightActive_[index]) {
				break;
			}
		}

		lightActive_[index] = true;
		PointLightData& light = effectBlock_->pointLightData_[index];

		//初期位置の抽選
		for (int i = 0; i < 3; ++i) {
			float max = initPosMax_[i];
			float min = initPosMin_[i];

			std::uniform_real_distribution<float> dist(min, max);

			light.position[i] = dist(rand_);
		}

		//色の抽選
		for (int i = 0; i < 3; ++i) {
			float min = 0.2f;
			float max = 1.0f;

			std::uniform_real_distribution<float> dist(min, max);

			light.color[i] = dist(rand_);
		}
		light.color.w = 0.6f;
	}


	//ライトの更新
	for (int i = 0; i < kPointLightNum_; ++i) {
		if (!lightActive_[i]) {
			continue;
		}

		PointLightData& light = effectBlock_->pointLightData_[i];
		lightLife_[i] += deltaTime;

		float t = 1.0f;
		if (lightLife_[i] < 1.0f) {
			t = std::sin(lightLife_[i] * (3.141592f / 2.0f));
		}else if(lightLife_[i] > kLightLife_ - 1.0f) {
			t = std::cos((lightLife_[i] - (kLightLife_ - 1.0f)) * (3.141592f / 2.0f));
		}

		//データ更新
		light.position.y -= deltaTime * kSpeed_;
		light.intensity = t * kLightIntensity_;

		//死亡処理
		if (lightLife_[i] >= kLightLife_) {
			lightActive_[i] = false;
			lightLife_[i] = 0.0f;
			light.intensity = 0.0f;
			continue;
		}
	}


	//ライトオブジェクトにライト情報を適用
	for (int i = 0; i < kPointLightNum_; ++i) {
		PointLightData& light = effectBlock_->pointLightData_[i];

		lightObj_->scale_[i] = light.intensity * Vector3(1.0f, 1.0f, 1.0f) * 1.0f;
		lightObj_->position_[i] = light.position;
		lightObj_->color_[i] = ConvertColor(light.color);
	}



	//Blockの回転処理
	for(int i = 0; i < kBlockNum_; ++i) {
		rotateSpeed_ *= kRotateAttenuation_;
		effectBlock_->afterRotate_[i] += rotateSpeed_ * deltaTime;
	}

	//データを挿入
	for (int i = 0; i < 3; ++i) {
		effectBlock_->directionalLightData_[i] = directionalLightData_[i];
	}
}

void RotateBlockEffect::Draw(Render* render) {
	render->Draw(effectBlock_.get());

	if (lightRender_) {
		render->Draw(lightObj_.get());
	}
}

void RotateBlockEffect::DrawImGui() {

	ImGui::Begin("RotateBlockEffect");

	ImGui::Text("Block");

	ImGui::Text("%d", blockEditIndex_);
	ImGui::SameLine();
	if (ImGui::Button("-")) { 
		blockEditIndex_--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) { 
		blockEditIndex_++;
	}

	blockEditIndex_ = std::clamp(blockEditIndex_, 0, kBlockNum_ - 1);
	colorBuffer_ = ConvertColor(effectBlock_->color_[blockEditIndex_]);
	outlineColorBuffer_ = ConvertColor(effectBlock_->outlineColor_[blockEditIndex_]);

	ImGui::DragFloat3("Position", &effectBlock_->position_[blockEditIndex_].x, 0.1f);
	ImGui::DragFloat3("Rotate", &effectBlock_->rotate_[blockEditIndex_].x, 0.05f);
	ImGui::DragFloat3("Scale", &effectBlock_->scale_[blockEditIndex_].x, 0.1f, 0.1f, 10.0f);

	ImGui::ColorEdit4("Color", &colorBuffer_.x);
	ImGui::ColorEdit4("OutlineColor", &outlineColorBuffer_.x);
	effectBlock_->color_[blockEditIndex_] = ConvertColor(colorBuffer_);
	effectBlock_->outlineColor_[blockEditIndex_] = ConvertColor(outlineColorBuffer_);

	if (ImGui::Button("Rotate")) {
		Rotation(2);
	}

	ImGui::DragFloat3("RotateSpeed", &kRotateSpeed_.x, 0.1f);
	ImGui::DragFloat("RotateAttenuation", &kRotateAttenuation_, 0.001f, 0.0f, 1.0f);

	//=================================================================

	ImGui::Separator();

	//=================================================================

	ImGui::Text("PointLight");

	int lifeNum = 0;
	for(const auto& active : lightActive_) {
		if(active) {
			lifeNum++;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Render")) {
		lightRender_ = !lightRender_;
	}

	ImGui::Text("%d / %d", lifeNum, kPointLightNum_);

	ImGui::DragFloat("Intensity", &kLightIntensity_, 0.01f, 0.0f);
	ImGui::DragFloat("Life", &kLightLife_, 0.1f, 0.1f);
	ImGui::DragFloat("Speed", &kSpeed_, 0.1f, 0.0f);
	ImGui::DragFloat("SpawnInterval", &spawnInterval_, 0.01f, 0.01f);

	ImGui::DragFloat3("InitPosMin", &initPosMin_.x, 0.1f);
	initPosMin_ = Vector3(
		std::min(initPosMin_.x, initPosMax_.x),
		std::min(initPosMin_.y, initPosMax_.y),
		std::min(initPosMin_.z, initPosMax_.z)
	);
	ImGui::DragFloat3("InitPosMax", &initPosMax_.x, 0.1f);
	initPosMax_ = Vector3(
		std::max(initPosMin_.x, initPosMax_.x),
		std::max(initPosMin_.y, initPosMax_.y),
		std::max(initPosMin_.z, initPosMax_.z)
	);

	//=================================================================

	ImGui::Separator();

	//=================================================================

	ImGui::Text("DirectionalLight");

	for (int i = 0; i < 3; ++i) {
		ImGui::DragFloat3(("Direction" + std::to_string(i)).c_str(), &directionalLightData_[i].direction.x, 0.01f);
		directionalLightData_[i].direction = directionalLightData_[i].direction.Normalize();
		ImGui::ColorEdit4(("Color" + std::to_string(i)).c_str(), &directionalLightData_[i].color.x);
		ImGui::DragFloat(("Intensity" + std::to_string(i)).c_str(), &directionalLightData_[i].intensity, 0.01f, 0.0f);
	}

	ImGui::End();

}

void RotateBlockEffect::Rotation(uint32_t level) {
	level = std::min(level, UINT(4));
	rotateSpeed_ = kRotateSpeed_ * float(level);
}
