#pragma once
#include <utility>
#include <Tools/KeyConfig/KeyManager.h>

//落ちてるテトリミノ。落下したらInitializeする。
class Tetrimino {
public:

	enum Type {
		None,
		I,
		O,
		S,
		Z,
		J,
		L,
		T,
		Count
	};

	Tetrimino() = default;
	~Tetrimino() = default;

	void Initialize(Type type);
	void Fall();
	void Controll(Key key);
	std::pair<int, int> GetBlockPos() const { return pos_; }

private:

	Type type_ = Type::None;
	std::pair<int, int> pos_ = { 0, 0 };

};
