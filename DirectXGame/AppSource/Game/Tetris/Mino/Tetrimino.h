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

	std::unordered_map<Type, std::vector<std::pair<int, int>>> blockOffsets_ = {
		{ Type::I, { {0, -1}, {0, 0}, {0, 1}, {0, 2} } },
		{ Type::O, { {0, 0}, {1, 0}, {0, 1}, {1, 1} } },
		{ Type::S, { {0, 0}, {1, 0}, {-1, 1}, {0, 1} } },
		{ Type::Z, { {-1, 0}, {0, 0}, {0, 1}, {1, 1} } },
		{ Type::J, { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} } },
		{ Type::L, { {1, -1}, {-1, 0}, {0, 0}, {1, 0} } },
		{ Type::T, { {0, -1}, {-1, 0}, {0, 0}, {1, 0} } }
	};

};
