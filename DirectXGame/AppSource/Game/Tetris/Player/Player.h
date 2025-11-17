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

	void SpawnMino(MovableMino moveMino) { 
		moveMino_ = moveMino; 
		hasMoveMino_ = true;
	}

	MovableMino GetMoveMino() const { return moveMino_; }

private:

	MovableMino moveMino_;

	Field* field_ = nullptr;

	float downTimer_ = 0.0f;

private:

	bool hasMoveMino_ = false;
	bool notAllowDown_ = false;

	bool isDown_ = false;
	bool reqFix_ = false;

};
