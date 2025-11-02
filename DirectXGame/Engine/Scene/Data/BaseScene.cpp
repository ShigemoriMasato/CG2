#include "BaseScene.h"
#include "SceneTools.h"

void BaseScene::MoveScene(SceneTools sceneTools, CommonData* commonData) {
	render_ = sceneTools.render_;
	input_ = sceneTools.input_;
	fpsObserver_ = sceneTools.fpsObserver_;

	this->commonData_ = commonData;
}

SceneTools BaseScene::CreateEngineTools() {
	SceneTools sceneTools;
	sceneTools.render_ = render_;
	sceneTools.input_ = input_;
	sceneTools.fpsObserver_ = fpsObserver_;
	return sceneTools;
}
