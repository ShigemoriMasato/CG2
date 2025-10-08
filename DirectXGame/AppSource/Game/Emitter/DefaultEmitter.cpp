#include "DefaultEmitter.h"
#include <stdlib.h>

int DefaultEmitter::idCounter_ = 0;
std::unique_ptr<BinaryManager> DefaultEmitter::binaryManager_ = std::make_unique<BinaryManager>();

DefaultEmitter::DefaultEmitter(int maxNum) : maxNum_(maxNum) {
	info_ = std::make_unique<ParticleResource>();
	info_->Initialize(4, 6, maxNum_);
	info_->localPos_ = {
		{-0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
	};
	info_->texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};
	info_->normal_ = {
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
	};
	info_->index_ = {
		0, 1, 2,
		1, 3, 2,
	};

	//乱数初期化
	std::random_device rd{};
	randEngine_ = std::mt19937(rd());

	//パーティクルの動き
	velocity_.resize(maxNum_);
	acceleration_.resize(maxNum_, { 0.0f, 0.0f, 0.0f });
	rotation_.resize(maxNum_, { 0.0f, 0.0f, 0.0f });
	life_.resize(maxNum_, lifeTime_);
	lifeSpan_.resize(maxNum_, lifeTime_);
	particleJobs_.resize(maxNum_);
	std::fill(info_->color_.begin(), info_->color_.end(), 0);

	timer_ = 0;
	emitterJobs_ = ParticleJob::ToTransparent | ParticleJob::Scatter;

	id_ = idCounter_++;
}

DefaultEmitter::~DefaultEmitter() {
}

void DefaultEmitter::Initialize(Camera* camera) {
	info_->psoConfig_.blendID = BlendStateID::Add;
	info_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	info_->psoConfig_.isSwapChain = true;

	info_->camera_ = camera;
}

void DefaultEmitter::Update() {

	if (isActive_) {
		timer_ += FPSObserver::GetDeltatime();
	}

	//==================- パーティクル発生処理 -==================
	while (timer_ > coolTime_) {
		timer_ -= coolTime_;

		for (int i = 0; i < generateNum_; ++i) {
			if(particleIndex_ >= maxNum_) {
				particleIndex_ = 0;
			}

			//新しく生成するパーティクルがまだ生きているなら生成しない
			if (info_->color_[particleIndex_] != 0){
				timer_ = 0.0f;
				break;
			}

			// =================- 新しいパーティクルのインデックスを更新 -==================
			particleNum_++;

			// =================- 新しいパーティクルの動作を更新 -==================
			particleJobs_[particleIndex_] = emitterJobs_;

			if (emitterJobs_ & ParticleJob::Scatter) {
				std::uniform_real_distribution distX(scatterMin_.x, scatterMax_.x);
				std::uniform_real_distribution distY(scatterMin_.y, scatterMax_.y);
				std::uniform_real_distribution distZ(scatterMin_.z, scatterMax_.z);

				velocity_[particleIndex_] = {
					distX(randEngine_),
					distY(randEngine_),
					distZ(randEngine_)
				};
			} else {
				velocity_[particleIndex_] = { 0.0f, 0.0f, 0.0f };
			}

			if (emitterJobs_ & ParticleJob::Gravity) {
				acceleration_[particleIndex_] = gravity_;
			}

			if (emitterJobs_ & ParticleJob::ToRotate) {
				float inverse = 1.0f;

				//ローテーションの向きをランダムに変える
				if (isRotateInverse_ && rand() % 2 == 0) {
					inverse = -1;
				}

				rotation_[particleIndex_].x -= rotateSpeed_.x * inverse;
				rotation_[particleIndex_].y -= rotateSpeed_.y * inverse;
				rotation_[particleIndex_].z -= rotateSpeed_.z * inverse;
			}



			info_->color_[particleIndex_] = ((rand() % 0xfff) << 20) | ((rand() % 0xfff) << 8) | 0xff;
			info_->position_[particleIndex_] = position_;
			life_[particleIndex_] = 0.0f;
			lifeSpan_[particleIndex_] = lifeTime_;

			++particleIndex_;
		}
	}


	//==================- パーティクル更新処理 -==================
	for (int i = 0; i < maxNum_; ++i) {
		//死んでいるパーティクルは更新しない
		if (life_[i] >= lifeSpan_[i]) {
			continue;
		}

		//生きた時間の更新
		life_[i] += FPSObserver::GetDeltatime();

		//だんだん透明に
		if (particleJobs_[i] & ParticleJob::ToTransparent) {
			info_->color_[i] = info_->color_[i] & 0xffffff00 | static_cast<uint32_t>(0xff * static_cast<float>(lifeSpan_[i] - life_[i]) / lifeSpan_[i]);
		}

		//加速度を加算
		velocity_[i] += acceleration_[i] * FPSObserver::GetDeltatime();
		
		//移動量を加算
		info_->position_[i] += velocity_[i] * FPSObserver::GetDeltatime();
		
		//回転量を加算
		info_->rotate_[i] += rotation_[i] * FPSObserver::GetDeltatime();

		if (life_[i] > lifeSpan_[i]) {
			info_->color_[i] = 0;
			--particleNum_;
		}
	}
}

void DefaultEmitter::Draw(Render* render) {
	render->Draw(info_.get());
}

void DefaultEmitter::SetLifeTime(float time) {
	lifeTime_ = time;
	std::fill(lifeSpan_.begin() + particleIndex_, lifeSpan_.end(), lifeTime_);
}

void DefaultEmitter::SetJobs(uint32_t jobs) {
	emitterJobs_ = jobs;
}

void DefaultEmitter::SetJobs(ParticleJob jobs) {
	emitterJobs_ = static_cast<uint32_t>(jobs);
}

void DefaultEmitter::SetPosition(Vector3 pos) {
	position_ = pos;
}

void DefaultEmitter::SetConfig(ParticleJob jobs, Vector3 value, Vector3 value2) {
	switch (jobs) {
	case ParticleJob::Gravity:
		//重力
		velocity_[particleIndex_].y -= value.y * FPSObserver::GetDeltatime();
		return;
	case ParticleJob::ToRotate:
		//回転
		velocity_[particleIndex_].x += value.x * FPSObserver::GetDeltatime();
		velocity_[particleIndex_].y += value.y * FPSObserver::GetDeltatime();
		velocity_[particleIndex_].z += value.z * FPSObserver::GetDeltatime();

		if (value2.Length() > 0.0f) {
			isRotateInverse_ = true;
		}

		return;
	case ParticleJob::Scatter:
		//散らばる範囲
		scatterMax_ = value;
		scatterMin_ = value2;
		return;
	}
}

void DefaultEmitter::SetParticleJobs(ParticleJob job, bool isUse) {
	if (isUse) {
		emitterJobs_ = emitterJobs_ | job;
	} else {
		//jobのビットをひっくり返す
		uint32_t inverseJob = ~static_cast<uint32_t>(job);
		//ひっくり返したビットとANDを取ることでjobのビットだけ0にできる
		emitterJobs_ = emitterJobs_ & inverseJob;
	}

}


//ImGui関連
void DefaultEmitter::DrawImGui() {
	ImGui::PushID(id_);
	ImGui::Begin("Emitter");

	//名前
	ImGui::InputText("EmitterName", nameBuf, 128);
	name_ = nameBuf;
	//パーティクルの生成数
	ImGui::Text("ParticleNum: %d / %d", particleNum_, maxNum_);

	//基本設定
	ImGui::Checkbox("Active", &isActive_);
	ImGui::InputFloat3("Position", &position_.x);
	ImGui::InputFloat("CoolTime", &coolTime_);
	ImGui::InputFloat("LifeTime", &lifeTime_);
	ImGui::InputInt("GenerateNum", &generateNum_);

	//保存に関する設定
	if (ImGui::CollapsingHeader("FileEdit")) {
		
		if(ImGui::Button("Save")) {
			Save();
		}

		if (ImGui::Button("Load")) {
			Load();
		}
	}


	ImGui::Separator();

	
	if (ImGui::CollapsingHeader("ToTransparent")) {
		bool toTransparent = (emitterJobs_ & ParticleJob::ToTransparent) != 0;
		if (ImGui::Checkbox("ToTransparent_", &toTransparent)) {
			SetParticleJobs(ParticleJob::ToTransparent, toTransparent);
		}
	}

	if (ImGui::CollapsingHeader("Scatter")) {
		bool scatter = (emitterJobs_ & ParticleJob::Scatter) != 0;
		if (ImGui::Checkbox("Scatter_", &scatter)) {
			SetParticleJobs(ParticleJob::Scatter, scatter);
		}
		ImGui::InputFloat3("scat_Max", &scatterMax_.x);
		ImGui::InputFloat3("scat_Min", &scatterMin_.x);
	}

	if (ImGui::CollapsingHeader("Acceleration")) {
		bool gravity = (emitterJobs_ & ParticleJob::Gravity) != 0;
		if (ImGui::Checkbox("Accelerate_", &gravity)) {
			SetParticleJobs(ParticleJob::Gravity, gravity);
		}
		ImGui::InputFloat3("AcceleratePow", &gravity_.x);
	}

	if (ImGui::CollapsingHeader("ToRotate")) {
		bool toRotate = (emitterJobs_ & ParticleJob::ToRotate) != 0;
		if (ImGui::Checkbox("ToRotate_", &toRotate)) {
			SetParticleJobs(ParticleJob::ToRotate, toRotate);
		}
		ImGui::InputFloat3("RotateSpeed", &rotateSpeed_.x);
		ImGui::Checkbox("Inverse", &isRotateInverse_);
	}


	ImGui::End();
	ImGui::PopID();
}


void DefaultEmitter::SetName(const std::string& name) {
	name_ = name;
	strncpy_s(nameBuf, name.c_str(), sizeof(nameBuf));
}

// =================- 外部ファイル系の処理 -==================
void DefaultEmitter::Save() {
	binaryManager_->RegistOutput(scatterMax_, "scatterMax");
	binaryManager_->RegistOutput(scatterMin_, "scatterMin");
	binaryManager_->RegistOutput(gravity_, "gravity");
	binaryManager_->RegistOutput(rotateSpeed_, "rotateSpeed");
	binaryManager_->RegistOutput(isRotateInverse_, "isRotateInverse");
	binaryManager_->RegistOutput(position_, "position");
	binaryManager_->RegistOutput(lifeTime_, "lifeTime");
	binaryManager_->RegistOutput(coolTime_, "coolTime");
	binaryManager_->RegistOutput(generateNum_, "generateNum");
	binaryManager_->RegistOutput(emitterJobs_, "emitterJobs");

	binaryManager_->Write(saveFileFolder + name_ + ".bin");
}

void DefaultEmitter::Load() {
	auto data = binaryManager_->Read(saveFileFolder + name_ + ".bin");

	//? よくわかってない
	// ハッシュ関数（コンパイル時定数）
	constexpr auto hash = [](const char* str) constexpr -> uint32_t {
		uint32_t hash = 2166136261u;
		while (*str) {
			hash ^= static_cast<uint32_t>(*str++);
			hash *= 16777619u;
		}
		return hash;
	};

	for (const auto& d : data) {
		switch (hash(d->name.c_str())) {
		case hash("scatterMax"):
			scatterMax_ = d->get<Vector3>();
			break;
		case hash("scatterMin"):
			scatterMin_ = d->get<Vector3>();
			break;
		case hash("gravity"):
			gravity_ = d->get<Vector3>();
			break;
		case hash("rotateSpeed"):
			rotateSpeed_ = d->get<Vector3>();
			break;
		case hash("isRotateInverse"):
			isRotateInverse_ = d->get<bool>();
			break;
		case hash("position"):
			position_ = d->get<Vector3>();
			break;
		case hash("lifeTime"):
			lifeTime_ = d->get<float>();
			std::fill(lifeSpan_.begin(), lifeSpan_.end(), lifeTime_);
			break;
		case hash("coolTime"):
			coolTime_ = d->get<float>();
			break;
		case hash("generateNum"):
			generateNum_ = d->get<int>();
			break;
		case hash("emitterJobs"):
			emitterJobs_ = d->get<uint32_t>();
			break;
		default:
			// 未知のパラメータの場合は何もしない
			break;
		}
	}
}

// =================- ParticleJobのビット演算子オーバーロード -==================
uint32_t operator|(ParticleJob a, ParticleJob b) {
	return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

uint32_t operator|(uint32_t a, ParticleJob b) {
	return a | static_cast<uint32_t>(b);
}

uint32_t operator&(uint32_t a, ParticleJob b) {
	return a & static_cast<uint32_t>(b);
}
