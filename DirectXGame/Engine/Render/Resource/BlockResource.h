#pragma once
#include "Data/BaseResource.h"
#include <Camera/Camera.h>

class BlockResource : public BaseResource {
public:

	void Initialize(uint32_t instance);
	void MakePointLight(uint32_t num);
	void MakeDirectionalAndPointLight(uint32_t directional, uint32_t point);

	void DrawReady() override;

	std::vector<Vector3> scale_;
	std::vector<Vector3> rotate_;
	std::vector<Vector3> afterRotate_;				//rotateした後にrotateできるやつ。ジンバルロック対策
	std::vector<Vector3> position_;

	std::vector<uint32_t> color_;
	std::vector<uint32_t> outlineColor_;

	std::vector<PointLightData> pointLightData_;
	std::vector<DirectionalLightData> directionalLightData_;

	Camera* camera_ = nullptr;

private:

	struct BlockData {
		Matrix4x4 world;
		Matrix4x4 vp;
		uint32_t color;
		uint32_t outlineColor;
		float padding[2];
		float padding2[4];
	};

	BlockData* blockData_ = nullptr;

	PointLightData* pointLights_ = nullptr;
	DirectionalLightData* directionalLights_ = nullptr;
};
