#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <random>

class Planets {
public:

	Planets() = default;
	~Planets();

	void Initialize(AssetsID textureIndex, Camera* camera, std::mt19937 rand);
	void Update(float deltaTime);
	void Draw(Render* render);
	void DrawImGui();

private:

	void Create();

	std::mt19937 rand_;

	std::unique_ptr<ParticleResource> planetParticles_ = nullptr;

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;

	std::vector<bool> active_{};
	std::vector<float> speed_{};


	float createTimer_ = 0.0f;

private://Config

	const int kPlanetNum_ = 4096;

	float createInterval_ = 0.01f;
	float kAcceleration_ = 5.0f;

	Vector2 initPosMin_ = { -20.0f, -15.0f };
	Vector2 initPosMax_ = { 20.0f, 15.0f };
	float initPosZ_ = -20.0f;

	Vector3 scale_ = { 3.0f, 3.0f, 3.0f };

	float deleteLine_ = 500.0f;

};
