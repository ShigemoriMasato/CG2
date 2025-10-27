#include "Field.h"

void Field::Initialize() {
	field_.resize(height_, std::vector<int>(width_, 0));
}

void Field::Update() {
	// フィールドの更新処理（例：ラインの消去など）をここに実装
}

void Field::Draw() {
	// フィールドの描画処理をここに実装
}
