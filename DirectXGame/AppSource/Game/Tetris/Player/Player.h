#pragma once
#include <Game/Tetris/Field/Field.h>
#include <Game/Tetris/Data.h>
#include <Game/Tetris/Mino/Tetrimino.h>
#include <Render/Resource/BlockResource.h>
#include <array>

class Player {
public:

	Player() = default;
	~Player() = default;

	void Initialize(Field* field, Tetrimino* tetrimino, Camera* camera);
	void Update(float deltaTime, std::unordered_map<Key, bool> key);
	void HoldDraw(Render* render);

	bool SpawnMino(Tetrimino::Type tetriminoType = Tetrimino::None);

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

	void PlayerControl(float deltaTime, std::unordered_map<Key, bool> key);

	void ExecuteSRS(Rotate rotate);

private:

	MovableMino moveMino_;
	Direction direction_ = Direction::dUp;
	std::pair<int, int> spawnPosition_;

	Tetrimino::Type holdMino_ = Tetrimino::Type::None;
	std::unique_ptr<BlockResource> holdRes_ = nullptr;
	Vector3 holdPosition_ = Vector3(-11.0f, 6.0f, 0.0f);

	Field* field_ = nullptr;
	Tetrimino* tetrimino_ = nullptr;

	float downTimer_ = 0.0f;

private:

	bool hasMoveMino_ = false;
	bool notAllowDown_ = false;

	bool isDown_ = false;
	bool reqFix_ = false;

	bool gameOver_ = false;
	bool holded_ = false;
};
