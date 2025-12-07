#pragma once
#include "Data/BaseResource.h"
#include <Tools/Binary/BinaryManager.h>

class WaterPlaneResource : public BaseResource {
public:

	~WaterPlaneResource();

	struct Wave {
		Vector2 direction;
		float amplitude;
		float wavelength;
		float speed;
		Vector3 pad;
	};

	void Initialize(AssetsID modelID);

	void DrawImGui();

	void DrawReady() override;

	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 position = { 0.0f, 0.0f, 0.0f };

	float time_ = 0.0f;

	std::vector<Wave> waves_{};

	Camera* camera_ = nullptr;

private:

	void Save();

	struct UniqueData {
		Matrix4x4 world;
		Matrix4x4 vp;
		Vector3 cameraPos;
		float time;
		int waveCount;
		Vector3 pad;
		Wave waves[8];
	};

	UniqueData* uniqueData_ = nullptr;

	DirectionalLightData* directLight_ = nullptr;

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;

};
