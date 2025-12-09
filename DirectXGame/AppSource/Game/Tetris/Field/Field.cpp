#include "Field.h"
#include <numbers>

namespace nums = std::numbers;

Field::Field(int width, int height) : width_(width), height_(height) {
}

void Field::Initialize(Camera* camera) {
	field_.resize(height_ + 5, std::vector<int>(width_, 0));

	debugLine_ = std::make_unique<ParticleResource>();
	debugLine_->Initialize(ShapeType::Cube, (width_ - 1) + height_);

	int index = 0;
	for (int j = 0; j < width_ - 1; ++j) {
		debugLine_->position_[index] = {
			float(j) - float(width_) / 2 + 0.5f, -0.5f, -0.5f
		};
		debugLine_->scale_[index] = { 0.05f, float(height_), 0.1f };
		debugLine_->color_[index] = 0x60606060;
		++index;
	}

	for (int i = 0; i < height_; ++i) {
		debugLine_->position_[index] = {
			-0.5f, float(i) - float(height_) / 2 + 0.5f, -0.5f
		};
		debugLine_->scale_[index] = { float(width_), 0.05f, 0.1f };
		debugLine_->color_[index] = 0x60606060;
		++index;
	}

	debugLine_->camera_ = camera;

	backPlane_ = std::make_unique<ColorfullPlaneResource>();
	std::vector<uint32_t> color = {
		0xe0, 0xe0, 0xffffffff, 0xffffffff
	};
	backPlane_->Initialize(color.data());
	backPlane_->scale_ = { float(width_), float(height_), 1.0f };
	backPlane_->position_ = { -0.5f, -0.5f, 0.5f };
	backPlane_->color_ = 0xff;
	backPlane_->camera_ = camera;
}

void Field::Draw(Render* render) {
	render->Draw(backPlane_.get());
	render->Draw(debugLine_.get());
}

void Field::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Field");

	ImGui::DragFloat3("BackPlane Position", &backPlane_->position_.x, 0.01f);
	ImGui::DragFloat3("BackPlane Scale", &backPlane_->scale_.x, 0.01f);

	Vector4 color = ConvertColor(backPlane_->color_);
	ImGui::ColorEdit4("Color", &color.x);
	backPlane_->color_ = ConvertColor(color);

	ImGui::End();
#endif
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

		if (isLineFull) {
			reqLine_.push_back(i);
		}
	}
}

void Field::DeleteFillLine() {
	int offset = 0;
	for (const auto& line : reqLine_) {
		field_.erase(field_.begin() + line - offset++);
		field_.emplace_back(std::vector<int>(width_, 0));
	}
	reqLine_.clear();
}
