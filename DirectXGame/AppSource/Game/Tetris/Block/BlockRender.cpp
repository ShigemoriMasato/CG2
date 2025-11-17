#include "BlockRender.h"

void BlockRender::Initialize(int width, int height, Camera* camera) {
	blockResource_ = std::make_unique<BlockResource>();
	blockResource_->Initialize(width * height);
	blockResource_->camera_ = camera;
	width_ = width;
	height_ = height;

	//Blockの座標固定
	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			int index = i * width_ + j;

			blockResource_->scale_[index] = { 1.0f, 1.0f, 1.0f };
			blockResource_->rotate_[index] = { 0.0f, 0.0f, 0.0f };
			blockResource_->position_[index] = {
				float(j) - float(width_) / 2, float(i) - float(height_) / 2, 0.0f
			};

			blockResource_->color_[index] = 0x00000000;
			blockResource_->outlineColor_[index] = 0x00000000;

		}
	}

	wallResource_ = std::make_unique<BlockResource>();
	wallResource_->Initialize((height_ + 1) * 2 + width_);
	wallResource_->camera_ = camera;

	int index = 0;
	for (int i = 0; i < height_ + 1; ++i) {
		for (int j = 0; j < 2; ++j) {
			wallResource_->position_[index] = {
				-width_ / 2.0f + (width_ + 1.0f) * j - 1.0f, -float(i) + float(height_) / 2 - 1.0f, 0.0f
			};
			wallResource_->scale_[index] = { 1.0f, 1.0f, 1.0f };
			wallResource_->color_[index] = 0xff;
			wallResource_->outlineColor_[index] = 0xffffffff;

			++index;
		}
	}

	for (int i = 0; i < width_; ++i) {
		wallResource_->position_[index] = {
			float(i) - float(width_) / 2, -float(height_) / 2 - 1.0f, 0.0f
		};
		wallResource_->scale_[index] = { 1.0f, 1.0f, 1.0f };
		wallResource_->color_[index] = 0xff;
		wallResource_->outlineColor_[index] = 0xffffffff;
		++index;
	}
}

void BlockRender::SetBlockConfig(std::map<int, BlockConfig> configMap) {
	configMap_ = configMap;
}

void BlockRender::SetBlock(int x, int y, int configIndex) {
	int index = y * width_ + x;
	if (configMap_.find(configIndex) != configMap_.end()) {
		blockResource_->color_[index] = configMap_[configIndex].color;
		blockResource_->outlineColor_[index] = configMap_[configIndex].outlineColor;
	} else {
		// 存在しないconfigIndexの場合、ブロックを非表示にする
		blockResource_->color_[index] = 0; // 完全透明
	}
}

void BlockRender::SetBlock(std::vector<std::vector<int>> allConfigIndices, MovableMino movableMino) {
	if (allConfigIndices.size() < static_cast<size_t>(height_) ||
		allConfigIndices[0].size() < static_cast<size_t>(width_)) {
		return; // サイズが足りない場合は何もしない
	}

	for (const auto& [x, y] : movableMino.positions) {
		allConfigIndices[y][x] = movableMino.minoType;
	}

	for (int y = 0; y < height_; ++y) {
		for (int x = 0; x < width_; ++x) {
			int configIndex = allConfigIndices[y][x];
			SetBlock(x, y, configIndex);
		}
	}
}

void BlockRender::Draw(Render* render) {
	render->Draw(blockResource_.get());
	render->Draw(wallResource_.get());
}
