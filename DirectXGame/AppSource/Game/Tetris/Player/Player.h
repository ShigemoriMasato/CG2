#pragma once
#include <Game/Tetris/Field/Field.h>
#include <Game/Tetris/Data.h>
#include <array>

class Player {
public:

	Player() = default;
	~Player() = default;

	void Initialize(Field* field);
	void Update(float deltaTime, std::unordered_map<Key, bool> key);

	void SpawnMino(MovableMino moveMino);

	MovableMino GetMoveMino() const { return moveMino_; }
	bool GetHasMoveMino() const { return hasMoveMino_; }

private:

	MovableMino moveMino_;
	std::pair<int, int> spawnPosition_;

	Field* field_ = nullptr;

	float downTimer_ = 0.0f;

private:

	bool hasMoveMino_ = false;
	bool notAllowDown_ = false;

	bool isDown_ = false;
	bool reqFix_ = false;

	bool gameOver_ = false;
};
