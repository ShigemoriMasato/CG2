#pragma once
#include <string>
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

class Block {
public:

	Block() = default;
	~Block() = default;

	void SetPosition(std::pair<int, int> pos) { pos = pos; }
	void Fix() { isFixed = true; }

	void Draw(Render* render);

private:

	std::pair<int, int> pos = { 0, 0 };

	//モデル
	std::unique_ptr<ModelResource> model = nullptr;

	//設置したかどうか
	bool isFixed = false;

};

