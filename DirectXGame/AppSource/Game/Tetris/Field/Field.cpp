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
				float(j) - float(width_) / 2, -float(i) + float(height_) / 2, 0.0f
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

void Field::Update() {
	
}

void Field::Draw(Render* render) {
	render->Draw(debugSphere_.get());
}

void Field::SpawnMino(std::vector<std::pair<int, int>> tetrimino, int minoType) {
	uint32_t index = 0;

	for (const auto& offset : tetrimino) {
		int x = spawnPosition_.first + offset.first;
		int y = spawnPosition_.second + offset.second;
		if (x >= 0 && x < width_ && y >= 0 && y < height_ + 5) {
			field_[y][x] = minoType; // ブロックを配置（1はブロックが存在することを示す)
			currentMino_[index++] = { x, y };
		}
	}
}
