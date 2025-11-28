#pragma once
#include <Render/Resource/BlockResource.h>
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <random>

class RotateBlockEffect {
public:

	RotateBlockEffect() = default;
	~RotateBlockEffect();

	void Initialize(uint32_t instance, Camera* camera, std::mt19937 rand);
	void Update(float deltaTime);
	void Draw(Render* render);
	void DrawImGui();

	void Rotation(uint32_t level);

private:

	std::unique_ptr<BinaryManager> binaryManager_;
	const std::string kBinaryFileName_ = "RotateBlockEffect.dat";
	std::mt19937 rand_;

	std::unique_ptr<BlockResource> effectBlock_ = nullptr;
	std::unique_ptr<ParticleResource> lightObj_ = nullptr;

	int kBlockNum_;
	const int kPointLightNum_ = 64;
	
	Vector3 rotateSpeed_{};
	std::vector<bool> lightActive_;
	std::vector<float> lightLife_;
	std::vector<Vector3> lightStartPos_;

private://Config

	Vector3 initPosMin_ = { -20.0f, 10.0f, 0.0f };
	Vector3 initPosMax_ = { 20.0f, 30.0f, 20.0f };

	float kLightLife_ = 5.0f;
	float kSpeed_ = 3.0f;
	float kLightIntensity_ = 0.3f;

	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 1.0f;

	Vector3 kRotateSpeed_ = { 0.0f, 20.0f, 0.0f };
	float kRotateAttenuation_ = 0.99f;

	std::vector<DirectionalLightData> directionalLightData_;

private://ImGui
	int blockEditIndex_ = 0;

	Vector4 colorBuffer_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 outlineColorBuffer_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool lightRender_ = false;
};
