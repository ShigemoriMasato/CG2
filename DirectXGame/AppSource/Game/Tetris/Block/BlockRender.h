#pragma once
#include <Render/Resource/BlockResource.h>
#include <Render/Render.h>
#include <Game/Tetris/Data.h>

#include "Delete/DeleteEffect.h"

struct BlockConfig {
	uint32_t color;
	uint32_t outlineColor;
};

class BlockRender {
public:

	void Initialize(int width, int height, Camera* camera);
	void SetBlockConfig(std::map<int, BlockConfig> configMap);

	void SetBlock(int x, int y, int configIndex);
	void SetBlock(std::vector<std::vector<int>> allConfigIndices, MovableMino movableMino);

	void Update(float deltaTime);
	void BeginDeleteEffect(std::vector<int> fillLines, std::vector<std::vector<int>> deletedField);
	void Draw(Render* render);

	bool GetIsEffecting() const { return isDeleting_; }

private:

	std::unique_ptr<BlockResource> blockResource_ = nullptr;
	std::unique_ptr<BlockResource> wallResource_ = nullptr;
	std::map<int, BlockConfig> configMap_;

	bool isDeleting_ = false;
	std::vector<int> deletingLines_{};

	int width_ = 0;
	int height_ = 0;

private:

	std::unique_ptr<DeleteEffect> deleteEffect_ = nullptr;
	std::vector<std::vector<int>> deletedField_{};

};
