#pragma once
#include <memory>
#include "Data/SceneTools.h"

class BaseScene;
class EngineTerminal;
class CommonData;

class SceneManager {
public:

	SceneManager(std::unique_ptr<BaseScene> firstScene, EngineTerminal* engine);
	//MoveScene済みのSceneがある場合
	SceneManager(std::unique_ptr<BaseScene> firstScene, BaseScene* createdScene);
	~SceneManager();

	void SetCommonData(CommonData* commonData);

	void Update();
	void Draw();

	std::shared_ptr<CommonData> commonData_ = nullptr;

private:

	SceneTools tools_;
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

};

