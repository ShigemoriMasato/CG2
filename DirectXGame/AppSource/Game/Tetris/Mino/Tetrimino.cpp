#include "Tetrimino.h"

void Tetrimino::Initialize(Type type, std::mt19937 g) {
    // vectorをシャッフル
    std::shuffle(bag_.begin(), bag_.end(), g);


}

void Tetrimino::pop() {

}

Tetrimino::Type Tetrimino::GetNextTetrimino(int nextNum) {
    return Type::None;
}