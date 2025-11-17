#pragma once
#include <Render/Resource/BlockResource.h>
#include <Render/Render.h>
#include <Game/Tetris/Data.h>

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

	void Draw(Render* render);

private:

	std::unique_ptr<BlockResource> blockResource_ = nullptr;
	std::unique_ptr<BlockResource> wallResource_ = nullptr;
	std::map<int, BlockConfig> configMap_;

	int width_ = 0;
	int height_ = 0;

};
