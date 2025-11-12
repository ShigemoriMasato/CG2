#include "KeyCoating.h"

void KeyCoating::Update(float deltaTime) {
	keyFlugs_ = keyManager_->GetKeyStates();

	for (Key key : { Key::Right, Key::Left, Key::Up, Key::Down }) {
		int index = static_cast<int>(key);
		if (keyFlugs_[key]) {
			// 押されている間タイマーを減らす
			timer[index] -= deltaTime;

			//一定時間押し続けたら
			if (timer[index] <= 0.0f) {
				keyFlugs_[key] = true;
				timer[index] = initialTime;
			} else {
				keyFlugs_[key] = false;
			}

			//Trigger状態の時は時間に関係なくtrueにする。
			if (!preFlugs_[index]) {
				keyFlugs_[key] = true;
			}

			preFlugs_[index] = keyFlugs_[key];

		} else {

			timer[index] = initialTime;

		}
	}
}
