#include "Tetris.h"

void Tetris::Initialize(KeyCoating* keys, Camera* camera) {
	field_ = std::make_unique<Field>(fieldWidth_, fieldHeight_);
	field_->Initialize(camera);

	blockRender_ = std::make_unique<BlockRender>();
	blockRender_->Initialize(fieldWidth_, fieldHeight_, camera);
	blockRender_->SetBlockConfig( {
		{ Tetrimino::Type::None, {0x00000, 0x00000000} }, // 空白		Air
		{ Tetrimino::Type::S, {0x00ff00ff, 0xffffffff} }, // 赤			S
		{ Tetrimino::Type::Z, {0xff0000ff, 0xffffffff} }, // 緑			Z
		{ Tetrimino::Type::T, {0xff00ffff, 0xffffffff} }, // 紫			T
		{ Tetrimino::Type::O, {0xffff00ff, 0xffffffff} }, // 黄			O
		{ Tetrimino::Type::I, {0x00ffffff, 0xffffffff} }, // 水色		I
		{ Tetrimino::Type::L, {0x0000ffff, 0xffffffff} }, // 青			L
		{ Tetrimino::Type::J, {0xff8000ff, 0xffffffff} }, // オレンジ		J
		{ Tetrimino::Type::Del, {0xffffffff, 0xffffffff} }  // 白		DeleteEffect
	} );

	tetrimino_ = std::make_unique<Tetrimino>();
	std::random_device rd;
	mt = std::mt19937(rd());
	tetrimino_->Initialize(mt, camera);

	player_ = std::make_unique<Player>();
	player_->Initialize(field_.get(), tetrimino_.get(), camera);

	keys_ = keys;
}

void Tetris::Update(float deltaTime) {
	auto key = keys_->GetKeyStates();
	deletedLine_ = 0;

	if (!blockRender_->GetIsEffecting()) {
		player_->SetDownTime(downInterval_);

		bool success = player_->SpawnMino();
		if (!success) {
			gameOver_ = true;
			return;
		}

		player_->Update(deltaTime, key);

		auto mapData = field_->GetField();
		blockRender_->SetBlock(mapData, player_->GetMoveMino());

		auto fillLines = field_->FillLineIndex();
		if (!fillLines.empty()) {
			field_->DeleteFillLine();
			blockRender_->BeginDeleteEffect(fillLines, field_->GetField());
			deletedLine_ = int(fillLines.size());
		}

	}

	blockRender_->Update(deltaTime);
}

void Tetris::Draw(Render* render) {
	field_->Draw(render);
	blockRender_->Draw(render);
	player_->HoldDraw(render);
	tetrimino_->DrawNext(render);
}
