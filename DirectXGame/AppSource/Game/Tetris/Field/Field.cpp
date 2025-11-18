#include "Field.h"
#include <numbers>

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
}

void Field::Draw(Render* render) {
	render->Draw(debugSphere_.get());
}

std::vector<std::vector<int>> Field::GetField() const {
	return field_;
}

bool Field::SetFieldIndex(int x, int y, int type) {
	bool gameover = true;
	if (field_[y][x] != 0) {
		gameover = false;
	}
	field_[y][x] = type;
	return gameover;
}

void Field::LineCheck() {
	for (int i = 0; i < height_; ++i) {
		bool isLineFull = true;
		for (int j = 0; j < width_; ++j) {
			if (field_[i][j] == 0) {
				isLineFull = false;
				break;
			}
		}

		if (!isLineFull) {
			reqLine_.push_back(i);
		}
	}
}
