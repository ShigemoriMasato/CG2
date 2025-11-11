#pragma once
#include <vector>
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>

class Field {
public:

	Field(int width, int height);
	~Field() = default;

	void Initialize(Camera* camera);
	void Update();
	void Draw(Render* render);

	void SpawnMino(std::vector<std::pair<int, int>> tetrimino, int minoType);

	std::vector<std::vector<int>> GetField() const { return field_; }

private:

	std::vector<std::vector<int>> field_;
	const int width_;
	const int height_;
	std::pair<int, int> spawnPosition_;

	std::unique_ptr<ParticleResource> debugSphere_ = nullptr;

};
