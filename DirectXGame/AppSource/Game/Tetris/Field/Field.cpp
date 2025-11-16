#include "Field.h"
#include <numbers>

namespace {
	bool canMove(std::array<std::pair<int, int>, 4> positions, const std::vector<std::vector<int>>& field, int dx, int dy) {
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

namespace nums = std::numbers;

Field::Field(int width, int height) : width_(width), height_(height) {
}

void Field::Initialize(Camera* camera) {
	field_.resize(height_ + 5, std::vector<int>(width_, 0));

	debugSphere_ = std::make_unique<ParticleResource>();
	debugSphere_->Initialize(ShapeType::Cube, width_ * height_);

	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			int index = i * width_ + j;
			debugSphere_->position_[index] = {
				float(j) - float(width_) / 2, -float(i) + float(height_) / 2 - 1.0f, 0.0f
			};
			debugSphere_->rotate_[index] = { nums::pi_v<float> / 4.0f, nums::pi_v<float> / 4.0f, 0.0f };
			debugSphere_->scale_[index] = { 0.1f, 0.1f, 0.1f };
			debugSphere_->color_[index] = 0x008888ff;	//薄めのシアン
		}
	}

	debugSphere_->camera_ = camera;

	spawnPosition_ = { width_ / 2, height_ - 3 };
}

void Field::Down() {
	isDown_ = true;
}

void Field::Update(float deltaTime, std::unordered_map<Key, bool> key) {
	//キーによる移動
	if (key[Key::Right]) {
		if (canMove(moveMino_, field_, 1, 0)) {
			//一個右に移動
			for (int i = 0; i < moveMino_.size(); ++i) {
				moveMino_[i].first += 1;
			}
		}
	} else if (key[Key::Left]) {
		if(canMove(moveMino_, field_, -1, 0)) {
			//一個左に移動
			for (int i = 0; i < moveMino_.size(); ++i) {
				moveMino_[i].first -= 1;
			}
		}
	}

	if (key[Key::HardDrop]) {
		//落下可能な距離を計算
		int dropDistance = 0;
		while(canMove(moveMino_, field_, 0, dropDistance)) {
			--dropDistance;
		}

		//一気に落下
		for (int i = 0; i < moveMino_.size(); ++i) {
			moveMino_[i].second += dropDistance + 1;
			hasMoveMino_ = false;
		}
		SetTetriminoToField();
	}

	//下キーを押しているときは自由落下できない様にする
	notAllowDown_ = key[Key::Down];

	if (key[Key::Down]) {
		downTimer_ += deltaTime;

		//todo レベルによって条件の値を変更する
		if (downTimer_ >= 0.1f) {
			downTimer_ = 0.0f;

			if(canMove(moveMino_, field_, 0, -1)) {
				//一個下に移動
				for (int i = 0; i < moveMino_.size(); ++i) {
					moveMino_[i].second -= 1;
				}

			} else {

				//! タイマーを起動、一定時間経過したらフィールドに固定する。何かしらのアクションが合ったらタイマーをリセット
				//! 今は一旦即固定にする
				SetTetriminoToField();

			}
		}

	} else {

		downTimer_ = 100.0f; //次押した時すぐに下に移動できるよう、大きな値にする

	}

	//自動落下
	if (isDown_ && !notAllowDown_) {

		isDown_ = false;

		if (canMove(moveMino_, field_, 0, -1)) {

			//一個下に移動
			for (int i = 0; i < moveMino_.size(); ++i) {
				moveMino_[i].second -= 1;
			}

		} else {
			//フィールドに固定
			SetTetriminoToField();
		}
	}
}

void Field::Draw(Render* render) {
	render->Draw(debugSphere_.get());
}

void Field::SpawnMino(std::vector<std::pair<int, int>> tetrimino, int minoType) {
	uint32_t index = 0;

	for (const auto& offset : tetrimino) {
		if (index >= moveMino_.size()) {
			break;
		}

		int x = spawnPosition_.first + offset.first;
		int y = spawnPosition_.second + offset.second;

		moveMino_[index++] = { x, y };
	}

	minoType_ = minoType;
	hasMoveMino_ = true;
}

std::vector<std::vector<int>> Field::GetField() const {
	auto ans = field_;

	for(int i = 0; i < 4; ++i) {
		int x = moveMino_[i].first;
		int y = moveMino_[i].second;
		if (x < 0 || x >= width_ || y < 0 || y >= height_) {
			continue;
		}
		ans[y][x] = minoType_;
	}

	return ans;
}

void Field::SetTetriminoToField() {
	for (int i = 0; i < moveMino_.size(); ++i) {
		int x = moveMino_[i].first;
		int y = moveMino_[i].second;
		if (x < 0 || x >= width_ || y < 0 || y >= height_) {
			continue;
		}
		field_[y][x] = minoType_;
	}
	hasMoveMino_ = false;
}
