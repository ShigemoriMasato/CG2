#pragma once
#include <Scene/Data/BaseScene.h>
#include <Scene/SceneManager.h>

class InitializeScene : public BaseScene {
public:

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	void CreateStageTexture();
	void LoadAllResources();

	std::vector<std::unique_ptr<SceneManager>> sceneList_;

	bool isfirst_ = true;
};

