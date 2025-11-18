#include "Player.h"

namespace {
	bool canMove(MovableMino mino, const std::vector<std::vector<int>>& field, int dx, int dy) {
		std::vector<std::pair<int, int>> positions(mino.offset.size());

		for(int i = 0; i < positions.size(); ++i) {
			positions[i].first = mino.offset[i].first + mino.position.first;
			positions[i].second = mino.offset[i].second + mino.position.second;
		}

		for (int i = 0; i < positions.size(); ++i) {
			int newX = positions[i].first + dx;
			int newY = positions[i].second + dy;
			// フィールドの範囲外かどうかをチェック
			if (newX < 0 || newX >= field[0].size() || newY < 0 || newY >= field.size()) {
				return false;
			}
			// ブロックが存在するかどうかをチェック
			if (field[newY][newX] != 0) {
				return false;
			}
		}
		return true;
	}
}

void Player::Initialize(Field* field, Tetrimino* tetrimino) {
	field_ = field;
	tetrimino_ = tetrimino;
	spawnPosition_ = std::make_pair(int(field->GetField()[0].size() / 2 - 1), static_cast<int>(field->GetField().size() - 7));
}

void Player::Update(float deltaTime, std::unordered_map<Key, bool> key) {
	auto fieldData = field_->GetField();

	//キーによる移動
	if (key[Key::Right]) {
		if (canMove(moveMino_, fieldData, 1, 0)) {
			//一個右に移動
			moveMino_.position.first += 1;
		}
	} else if (key[Key::Left]) {
		if (canMove(moveMino_, fieldData, -1, 0)) {
			//一個左に移動
			moveMino_.position.first -= 1;
		}
	}

	if (key[Key::HardDrop]) {
		//落下可能な距離を計算
		int dropDistance = 0;
		while (canMove(moveMino_, fieldData, 0, dropDistance)) {
			--dropDistance;
		}

		//一気に落下
		moveMino_.position.second += dropDistance + 1;

		reqFix_ = true;

	}

	//下キーを押しているときは自由落下できない様にする
	notAllowDown_ = key[Key::Down];

	if (key[Key::Down]) {
		downTimer_ += deltaTime;

		//todo レベルによって条件の値を変更する
		if (downTimer_ >= 0.1f) {
			downTimer_ = 0.0f;

			if (canMove(moveMino_, fieldData, 0, -1)) {
				//一個下に移動
				moveMino_.position.second -= 1;

			} else {

				//! タイマーを起動、一定時間経過したらフィールドに固定する。何かしらのアクションが合ったらタイマーをリセット
				//! 今は一旦即固定にする
				reqFix_ = true;

			}
		}

	} else {

		downTimer_ = 100.0f; //次押した時すぐに下に移動できるよう、大きな値にする

	}

	if (key[Key::LRotate]) {
		ExecuteSRS(rLeft);
	}
	if (key[Key::RRotate]) {
		ExecuteSRS(rRight);
	}

	//自動落下
	if (isDown_ && !notAllowDown_) {

		isDown_ = false;

		if (canMove(moveMino_, fieldData, 0, -1)) {

			//一個下に移動
			moveMino_.position.second -= 1;

		} else {
			//フィールドに固定
			reqFix_ = true;
		}
	}

	if (reqFix_) {
		for (const auto& [x, y] : moveMino_.offset) {
			int newx = x + moveMino_.position.first;
			int newy = y + moveMino_.position.second;
			gameOver_ = field_->SetFieldIndex(newx, newy, moveMino_.minoType);
		}
		field_->LineCheck();
		hasMoveMino_ = false;
		reqFix_ = false;
	}
}

bool Player::SpawnMino() {
	if (hasMoveMino_) {
		return true;
	}

	moveMino_.minoType = tetrimino_->PopFirst();
	moveMino_.offset = tetrimino_->GetOffset(Tetrimino::Type(moveMino_.minoType));
	moveMino_.position = spawnPosition_;

	auto field = field_->GetField();

	//スポーン位置に移動
	for (int i = 0; i < moveMino_.offset.size(); ++i) {
		int x = moveMino_.offset[i].first + spawnPosition_.first;
		int y = moveMino_.offset[i].second + spawnPosition_.second;

		//スポーン位置にブロックがある場合、ゲームオーバー
		if (field[y][x] != 0) {
			gameOver_ = true;
			moveMino_ = {};
			return false;
		}
	}

	hasMoveMino_ = true;
	return true;
}

void Player::ExecuteSRS(Rotate rotate) {

	int prevDir = int(direction_);
	auto prevMino = moveMino_;
	int dir = (int(rotate) * 2 - 1 + int(direction_)) % 4;
	if (dir < 0) {
		dir += 4;
	}

	if(moveMino_.minoType == Tetrimino::Type::O) {
		//O型は回転しない
		return;
	} else if (moveMino_.minoType == Tetrimino::Type::I) {
		//I型の回転処理

		if (dir % 2 == 0) {
			int height = dir / 2;
			for (int i = 0; i < 4; ++i) {
				moveMino_.offset[i] = {
					i - 1,
					height - 1
				};
			}
		} else {
			int width = (dir / 2 - 1) * -1;
			for (int i = 0; i < 4; ++i) {
				moveMino_.offset[i] = {
					width,
					1 - i
				};
			}
		}

	} else {

		for (auto& pos : moveMino_.offset) {
			int i = pos.first;
			int j = pos.second;

			if (rotate == rRight) {
				pos.first = j;
				pos.second = -i;
			} else if (rotate == rLeft) {
				pos.first = -j;
				pos.second = i;
			}
		}

	}

	//SRSによる位置調整
	auto srs = tetrimino_->GetSRSOffset(prevDir, rotate == rRight);

	auto field = field_->GetField();
	bool moved = false;
	for (const auto& [dx, dy] : srs) {
		if (canMove(moveMino_, field, dx, dy)) {
			moveMino_.position.first += dx;
			moveMino_.position.second += dy;
			direction_ = Direction(dir);
			moved = true;
			break;
		}
	}

	//移動できなかったら元に戻す
	if (!moved) {
		moveMino_ = prevMino;
	}

	direction_ = Direction(dir);
}
