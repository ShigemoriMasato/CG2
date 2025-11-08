#pragma once
#include "../Func/MyMath.h"
#include <memory>

using namespace Matrix;
struct Transform;

//初期値を画面サイズ1280*720と仮定して設定

struct PerspectiveFovDesc {
	float fovY = 0.45f;
	float aspectRatio = 1280.0f / 720.0f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
};

struct OrthographicDesc {
	float left = -0.0f;
	float top = 0.0f;
	float right = 1280;
	float bottom = 720;
	float nearClip = 0.0f;
	float farClip = 1000.0f;
};

class Camera {
public:

	Camera() = default;
	~Camera() = default;
	
	void SetProjectionMatrix(PerspectiveFovDesc desc);
	void SetProjectionMatrix(OrthographicDesc desc);

	void MakeMatrix();
	void DrawImGui();

	void SetTransform(Transform transform);
	void SetTransform(Matrix4x4 mat);

	Matrix4x4 GetVPMatrix() const;

	virtual Vector3 GetPosition() const { return transform_.position; }
	Transform GetTransform() const { return transform_; }
	Matrix4x4 GetTranformMatrix() const { return transformMatrix_; }
	Matrix4x4 GetProjectionMatrix() const { return projectionMatrix; }

protected:

	Matrix4x4 projectionMatrix = Matrix::MakeIdentity4x4();		//ワールド行列にこれをかけると正射影になる
	Matrix4x4 vpMatrix = Matrix::MakeIdentity4x4();			//w抜きviewport変換行列

	Transform transform_ = {};							//カメラ座標
	
	Matrix4x4 transformMatrix_ = Matrix::MakeIdentity4x4();	//カメラ座標変換行列

	bool isSetMatrix = false;
};
