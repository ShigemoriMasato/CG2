#include "BaseScene.h"
#include "SceneTools.h"

void BaseScene::MoveScene(SceneTools sceneTools, CommonData* commonData) {
	render_ = sceneTools.render_;
	input_ = sceneTools.input_;
	offScreenManager_ = sceneTools.offScreenManager_;
	fpsObserver_ = sceneTools.fpsObserver_;
	assetsLoader_ = sceneTools.assetsLoader_;

	this->commonData = commonData;
}

SceneTools BaseScene::CreateEngineTools() {
	SceneTools sceneTools;
	sceneTools.render_ = render_;
	sceneTools.input_ = input_;
	sceneTools.offScreenManager_ = offScreenManager_;
	sceneTools.fpsObserver_ = fpsObserver_;
	sceneTools.assetsLoader_ = assetsLoader_;
	return sceneTools;
}
