#include "Tetrimino.h"
#include <stdlib.h>

void Tetrimino::Initialize(std::mt19937 g) {
    radg_ = g;
	nexts_.clear();
	RefillNextBuffer();
	nexts_ = nextBuffer_;
	RefillNextBuffer();
}

std::vector<std::pair<int, int>> Tetrimino::GetOffset(Type type) {
    return blockOffsets_[type];
}

Tetrimino::Type Tetrimino::PopFirst() {
	Type first = nexts_.front();
	nexts_.erase(nexts_.begin());

    while (nexts_.empty()) {
		nexts_ = nextBuffer_;
		RefillNextBuffer();
    }
    
    return first;
}

Tetrimino::Type Tetrimino::GetNextTetrimino(int nextNum) {
    if (nextNum < 0 || nextNum > 6) {
		return Type::None;
    }

    if (nexts_.size() > nextNum) {
        return nexts_[nextNum];
    } else {
        nextNum -= int(nexts_.size());
        return nextBuffer_[nextNum];
    }
}

void Tetrimino::RefillNextBuffer() {
    nextBuffer_.clear();
    std::vector<Type> allTypes = { S, Z, T, O, I, L, J };
    //1,2,3,4,5,6,7の最小公倍数:420
    std::uniform_int_distribution<int> dist(1, 420);

    for (int i = kMinoTypeNum_; i > 0; --i) {
		int randIndex = dist(radg_) % i;
        nextBuffer_.push_back(allTypes[randIndex]);
		allTypes.erase(allTypes.begin() + randIndex);
	}
}
