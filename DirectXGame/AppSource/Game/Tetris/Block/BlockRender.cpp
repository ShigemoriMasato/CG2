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

void BlockRender::SetBlock(std::vector<std::vector<int>> allConfigIndices) {
	if (allConfigIndices.size() < static_cast<size_t>(height_) ||
		allConfigIndices[0].size() < static_cast<size_t>(width_)) {
		return; // サイズが足りない場合は何もしない
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
}
