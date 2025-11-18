#pragma once
#include <Game/Tetris/Field/Field.h>
#include <Game/Tetris/Block/BlockRender.h>
#include <Game/Tetris/Mino/Tetrimino.h>
#include <Game/KeyState/KeyCoating.h>
#include <Game/Tetris/Player/Player.h>

class Tetris {
public:

	Tetris() = default;
	~Tetris() = default;

	void Initialize(KeyCoating* keys, Camera* camera);
	void Update(float deltaTime);
	void Draw(Render* render);

	bool IsGameOver() const { return gameOver_; }

private://定数

	const int fieldWidth_ = 10;
	const int fieldHeight_ = 20;

	std::mt19937 mt;

private:

	KeyCoating* keys_ = nullptr;

	std::unique_ptr<Field> field_ = nullptr;
	std::unique_ptr<BlockRender> blockRender_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Tetrimino> tetrimino_ = nullptr;

	bool gameOver_ = false;

};

