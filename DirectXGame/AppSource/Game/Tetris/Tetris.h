#pragma once
#include <Game/Tetris/Field/Field.h>
#include <Game/Tetris/Block/BlockRender.h>
#include <Game/Tetris/Mino/Tetrimino.h>

class Tetris {
public:

	Tetris() = default;
	~Tetris() = default;

	void Initialize(Camera* camera);
	void Update(float deltaTime);
	void Draw(Render* render);

private://定数

	const int fieldWidth_ = 10;
	const int fieldHeight_ = 20;

	std::mt19937 mt;

private:

	std::unique_ptr<Field> field_ = nullptr;
	std::unique_ptr<BlockRender> blockRender_ = nullptr;

	std::unique_ptr<Tetrimino> tetrimino_ = nullptr;

};

