#pragma once
#include <utility>
#include <Tools/KeyConfig/KeyManager.h>
#include <random>

//テトリミノに関する管理。7種1順をここで管理する
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

	void Initialize(Type type, std::mt19937 g);

	void pop();

	/// <summary>
	/// 次のテトリミノを取得する
	/// </summary>
	/// <param name="nextNum">16まで</param>
	Type GetNextTetrimino(int nextNum);

private:

	std::unordered_map<Type, std::vector<std::pair<int, int>>> blockOffsets_ = {
		{ Type::I, { {0, -1}, {0, 0}, {0, 1}, {0, 2} } },
		{ Type::O, { {0, 0}, {1, 0}, {0, 1}, {1, 1} } },
		{ Type::S, { {0, 0}, {1, 0}, {-1, 1}, {0, 1} } },
		{ Type::Z, { {-1, 0}, {0, 0}, {0, 1}, {1, 1} } },
		{ Type::J, { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} } },
		{ Type::L, { {1, -1}, {-1, 0}, {0, 0}, {1, 0} } },
		{ Type::T, { {0, -1}, {-1, 0}, {0, 0}, {1, 0} } }
	};

	std::vector<Type> nexts_;
	std::vector<Type> bag_ = { Type::I, Type::O, Type::S, Type::Z, Type::J, Type::L, Type::T };

};
