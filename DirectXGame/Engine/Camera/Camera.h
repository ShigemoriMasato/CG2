#pragma once
#include "../Math/MyMath.h"
#include <memory>

using namespace Matrix;
struct Transform;

//初期値を画面サイズ1280*720と仮定して設定

struct PerspectiveFovDesc {
	float fovY = 0.45f;
	float aspectRatio = 1280.0f / 720.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;
};

struct OrthographicDesc {
	float left = -640;
	float top = 640;
	float right = -360;
	float bottom = 360;
	float nearClip = 0.0f;
	float farClip = 1000.0f;
};

class Camera {
public:

	Camera() : transform_(std::make_shared<Transform>()) {};
	~Camera() = default;
	
	void SetProjectionMatrix(PerspectiveFovDesc desc);
	void SetProjectionMatrix(OrthographicDesc desc);

	void MakeMatrix();

	void SetTransform(Transform* transform);
	void SetTransform(Matrix4x4 mat);

	Matrix4x4 VPMatrix() const;

	virtual Vector3 GetPosition() const {
		return transform_->position;
	}
	Transform GetTransform() const { return *transform_; }

private:

	Matrix4x4 projectionMatrix = Matrix::MakeIdentity4x4();		//ワールド行列にこれをかけると正射影になる
	Matrix4x4 vpMatrix = Matrix::MakeIdentity4x4();			//w抜きviewport変換行列

	std::shared_ptr<Transform> transform_ = {};							//カメラ座標
	Matrix4x4 transformMatrix_ = Matrix::MakeIdentity4x4();	//カメラ座標変換行列

	bool isSetMatrix = false;
};
