#pragma once
#include "Data/BaseScene.h"
#include <Render/Resource/ModelResource.h>
#include <Camera/DebugCamera.h>
#include <Tools/Grid/GridMaker.h>
#include <Game/Emitter/DefaultEmitter.h>
#include <Render/ImGuiWrapper.h>

class TitleScene : public BaseScene {
public:

	TitleScene();
	~TitleScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	int descHandle_ = -1;
	std::unique_ptr<ModelResource> descModel_ = nullptr;
	Transform descTransform_;

	std::unique_ptr<DebugCamera> camera_ = nullptr;

	std::list<std::unique_ptr<GridMaker>> gridMaker_;
	std::unique_ptr<ParticleResource> triangle_ = nullptr;
	std::unique_ptr<DefaultEmitter> testEmitter_ = nullptr;

	std::unique_ptr<ImGuiWrapper> imguiWrapper_ = nullptr;
	Matrix4x4 worldMatrix_ = Matrix::MakeIdentity4x4();
	Transform gizmoTransform_;

	int mp3Handle;
	int wavHandle;

	std::unordered_map<Key, bool> keys_{};
};
