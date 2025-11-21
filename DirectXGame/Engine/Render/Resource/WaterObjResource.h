#pragma once
#include "Data/BaseResource.h"

class WaterObjResource : public BaseResource {
public:

	WaterObjResource();
	~WaterObjResource() = default;

	void Initialize(AssetsID id);

	void DrawReady() override;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	float waterHeight;
	uint32_t color;
	uint32_t textureIndex;

	Camera* camera_ = nullptr;

private:

	struct UniqueData {
		Vector4 color = {};
		uint32_t textureIndex_ = 0;
		float waterHeight = 0.0f;
		float padding[2];
	};

	MatrixData* matrixData_ = nullptr;
	UniqueData* uniqueData_ = nullptr;

};
