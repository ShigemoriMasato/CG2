#pragma once
#include <Scene/Data/BaseScene.h>
#include <Camera/DebugCamera.h>
#include "../Tetris/Tetris.h"

class GameScene : public BaseScene {
public:

	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<Tetris> tetris_ = nullptr;

};
