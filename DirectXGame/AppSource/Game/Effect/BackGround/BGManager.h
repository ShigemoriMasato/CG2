#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include "Planet/Planets.h"
#include <random>

class BGManager {
public:

	BGManager() = default;
	~BGManager() = default;

	void Initialize(AssetsLoader* loader, Camera* camera, std::mt19937 rand);
	void Update(float deltaTime);
	void Draw(Render* render);
	void DrawImGui();

private:

	std::unique_ptr<DrawResource> bgRes_ = nullptr;

	std::unique_ptr<ParticleResource> bgParticles_ = nullptr;

	std::unique_ptr<Planets> planets_ = nullptr;

};
