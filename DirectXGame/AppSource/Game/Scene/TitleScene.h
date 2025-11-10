#pragma once
#include <Scene/Data/BaseScene.h>
#include <Camera/DebugCamera.h>
#include <Render/ImGuiWrapper.h>
#include <Render/Resource/ParticleResource.h>

class TitleScene : public BaseScene {
public:

	TitleScene();
	~TitleScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	int descHandle_ = -1;
	Transform descTransform_;

	std::unique_ptr<DebugCamera> camera_ = nullptr;

	Matrix4x4 worldMatrix_ = Matrix::MakeIdentity4x4();
	Transform gizmoTransform_;

	std::unique_ptr<DrawResource> model_ = nullptr;
	std::unique_ptr<ParticleResource> particleRes_ = nullptr;

	int mp3Handle;
	int wavHandle;

	std::unordered_map<Key, bool> keys_{};

private:
	std::unique_ptr<Camera> camera2D_ = nullptr;
	int textureHandle_ = -1;
};
