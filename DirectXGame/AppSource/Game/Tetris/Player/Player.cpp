#include "Player.h"

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

void Player::Initialize(Field* field) {
}

void Player::Update(float deltaTime, std::unordered_map<Key, bool> key) {
	//キーによる移動
	if (key[Key::Right]) {
		if (canMove(moveMino_.positions, field_->GetField(), 1, 0)) {
			//一個右に移動
			for (int i = 0; i < moveMino_.positions.size(); ++i) {
				moveMino_.positions[i].first += 1;
			}
		}
	} else if (key[Key::Left]) {
		if (canMove(moveMino_.positions, field_->GetField(), -1, 0)) {
			//一個左に移動
			for (int i = 0; i < moveMino_.positions.size(); ++i) {
				moveMino_.positions[i].first -= 1;
			}
		}
	}

	if (key[Key::HardDrop]) {
		//落下可能な距離を計算
		int dropDistance = 0;
		while (canMove(moveMino_.positions, field_->GetField(), 0, dropDistance)) {
			--dropDistance;
		}

		//一気に落下
		for (int i = 0; i < moveMino_.positions.size(); ++i) {
			moveMino_.positions[i].second += dropDistance + 1;
			hasMoveMino_ = false;
		}

		reqFix_ = true;

	}

	//下キーを押しているときは自由落下できない様にする
	notAllowDown_ = key[Key::Down];

	if (key[Key::Down]) {
		downTimer_ += deltaTime;

		//todo レベルによって条件の値を変更する
		if (downTimer_ >= 0.1f) {
			downTimer_ = 0.0f;

			if (canMove(moveMino_.positions, field_->GetField(), 0, -1)) {
				//一個下に移動
				for (int i = 0; i < moveMino_.positions.size(); ++i) {
					moveMino_.positions[i].second -= 1;
				}

			} else {

				//! タイマーを起動、一定時間経過したらフィールドに固定する。何かしらのアクションが合ったらタイマーをリセット
				//! 今は一旦即固定にする
				reqFix_ = true;

			}
		}

	} else {

		downTimer_ = 100.0f; //次押した時すぐに下に移動できるよう、大きな値にする

	}

	//自動落下
	if (isDown_ && !notAllowDown_) {

		isDown_ = false;

		if (canMove(moveMino_.positions, field_->GetField(), 0, -1)) {

			//一個下に移動
			for (int i = 0; i < moveMino_.positions.size(); ++i) {
				moveMino_.positions[i].second -= 1;
			}

		} else {
			//フィールドに固定
			reqFix_ = true;
		}
	}

}
