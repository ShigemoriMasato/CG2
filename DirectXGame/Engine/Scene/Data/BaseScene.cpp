#include "BaseScene.h"
#include "SceneTools.h"

void BaseScene::MoveScene(SceneTools sceneTools, CommonData* commonData) {
	modelManager_ = sceneTools.modelManager_;
	textureManager_ = sceneTools.textureManager_;
	render_ = sceneTools.render_;
	input_ = sceneTools.input_;
	audio_ = sceneTools.audio_;
	offScreenManager_ = sceneTools.offScreenManager_;
	fpsObserver_ = sceneTools.fpsObserver_;

	this->commonData = commonData;
}

SceneTools BaseScene::CreateEngineTools() {
	SceneTools sceneTools;
	sceneTools.modelManager_ = modelManager_;
	sceneTools.textureManager_ = textureManager_;
	sceneTools.render_ = render_;
	sceneTools.input_ = input_;
	sceneTools.audio_ = audio_;
	sceneTools.offScreenManager_ = offScreenManager_;
	sceneTools.fpsObserver_ = fpsObserver_;
	return sceneTools;
}
