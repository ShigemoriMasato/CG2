#pragma once
#include "Data/BaseResource.h"
#include <Camera/Camera.h>

class ParticleResource : public BaseResource {
public:

	ParticleResource() = default;
	~ParticleResource() = default;

	void Initialize(ShapeType type, uint32_t instanceNum);

	void DrawReady() override;

	std::vector<uint32_t> index_{};

	std::vector<Vector3> position_{};
	std::vector<Vector3> rotate_{};
	std::vector<Vector3> scale_{};

	std::vector<AssetsID> textureIndex_{};
	std::vector<uint32_t> color_ = {};

	Camera* camera_ = nullptr;

private:

	struct ParticleData final {
		Matrix4x4 vp;
		Matrix4x4 world;
		uint32_t color;
		uint32_t textureIndex;
		Vector2 padding;
		Vector4 pad;
	};

	VertexData* vertex_ = nullptr;
	uint32_t* indices_ = nullptr;
	ParticleData* particleData_ = nullptr;

};
