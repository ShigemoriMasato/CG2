#pragma once
#include <Game/Tetris/Field/Field.h>
#include <Game/Tetris/Data.h>
#include <Game/Tetris/Mino/Tetrimino.h>
#include <array>

class Player {
public:

	Player() = default;
	~Player() = default;

	void Initialize(Field* field, Tetrimino* tetrimino);
	void Update(float deltaTime, std::unordered_map<Key, bool> key);

	bool SpawnMino();

	MovableMino GetMoveMino() const { return moveMino_; }
	bool GetHasMoveMino() const { return hasMoveMino_; }

private:

	enum Rotate {
		rLeft,
		rRight
	};

	enum Direction {
		dUp,
		dRight,
		dDown,
		dLeft,
	};

private:

	void ExecuteSRS(Rotate rotate);

private:

	MovableMino moveMino_;
	Direction direction_ = Direction::dUp;
	std::pair<int, int> spawnPosition_;

	Field* field_ = nullptr;
	Tetrimino* tetrimino_ = nullptr;

	float downTimer_ = 0.0f;

private:

	bool hasMoveMino_ = false;
	bool notAllowDown_ = false;

	bool isDown_ = false;
	bool reqFix_ = false;

	bool gameOver_ = false;
};
