#pragma once
#include <Render/Resource/BlockResource.h>
#include <Render/Render.h>

class RotateBlockEffect {
public:

	RotateBlockEffect() = default;
	~RotateBlockEffect() = default;

	void Initialize(uint32_t instance);
	void Update(float deltaTime);
	void Draw(Render* render);

private:

	std::unique_ptr<BinaryManager> binaryManager_;

	std::unique_ptr<BlockResource> effectBlock_ = nullptr;
	
};
