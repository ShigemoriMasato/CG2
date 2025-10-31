#pragma once
#include "../Math/MyMath.h"
#include "Camera.h"
#include "../Render/Render.h"

class DebugCamera : public Camera {
public:

	void Initialize();

	void Update();

	Vector3 GetPosition() const override;
	Vector3 GetCenter() const;

	void SetCenter(const Vector3& center) {
		center_ = center;
	}

private:

	//カメラの親座標
	Vector3 center_{};

	//カメラの球面座標系
	Vector3 spherical_{};

	//親からの距離
	float distance_ = -10.0f;

	const float speed_ = 0.003f;
};
