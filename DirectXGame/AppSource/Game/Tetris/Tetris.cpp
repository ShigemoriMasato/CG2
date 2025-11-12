#include "Tetris.h"

void Tetris::Initialize(KeyCoating* keys, Camera* camera) {
	field_ = std::make_unique<Field>(fieldWidth_, fieldHeight_);
	field_->Initialize(camera);

	blockRender_ = std::make_unique<BlockRender>();
	blockRender_->Initialize(fieldWidth_, fieldHeight_, camera);
	blockRender_->SetBlockConfig( {
		{ 0, {0x00000000, 0x00000000} }, // 空白		Air
		{ 1, {0x00ff00ff, 0xffffffff} }, // 赤		N
		{ 2, {0xff0000ff, 0xffffffff} }, // 緑		Z
		{ 3, {0xff00ffff, 0xffffffff} }, // 紫		T
		{ 4, {0xffff00ff, 0xffffffff} }, // 黄		O
		{ 5, {0x00ffffff, 0xffffffff} }, // 水色		I
		{ 6, {0x0000ffff, 0xffffffff} }, // 青		L
		{ 7, {0xff8000ff, 0xffffffff} }, // オレンジ	J
		{ 8, {0xffffffff, 0xffffffff} }  // 白		Wall
	} );

	tetrimino_ = std::make_unique<Tetrimino>();
	std::random_device rd;
	mt = std::mt19937(rd());
	tetrimino_->Initialize(mt);

	keys_ = keys;
}

void Tetris::Update(float deltaTime) {
	auto key = keys_->GetKeyStates();

	field_->Update();

	if (key[Key::Correct]) {
		Tetrimino::Type next = tetrimino_->PopFirst();
		auto offsets = tetrimino_->GetOffset(next);
		field_->SpawnMino(offsets, static_cast<int>(next));

		auto mapData = field_->GetField();
		blockRender_->SetBlock(mapData);
	}
}

void Tetris::Draw(Render* render) {
	field_->Draw(render);
	blockRender_->Draw(render);
}
