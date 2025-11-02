#pragma once
#include "CommonData.h"
#include <Core/EngineTerminal.h>

struct SceneTools;

class BaseScene {
public:

	BaseScene() = default;
	virtual ~BaseScene() = default;
	virtual void Initialize() = 0;
	virtual std::unique_ptr<BaseScene> Update() = 0;
	virtual void Draw() = 0;
	void MoveScene(SceneTools sceneTools, CommonData* commonData);
	SceneTools CreateEngineTools();

protected:

	CommonData* commonData_ = nullptr;

	Render* render_ = nullptr;
	Input* input_ = nullptr;
	FPSObserver* fpsObserver_ = nullptr;

};
