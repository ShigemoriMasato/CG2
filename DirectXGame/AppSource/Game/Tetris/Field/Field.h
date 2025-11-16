#pragma once
#include <vector>
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <Common/KeyConfig/KeyManager.h>

class Field {
public:

	Field(int width, int height);
	~Field() = default;

	void Initialize(Camera* camera);

	void Down();

	void Update(float deltaTime, std::unordered_map<Key, bool> key);
	void Draw(Render* render);

	void SpawnMino(std::vector<std::pair<int, int>> tetrimino, int minoType);

	std::vector<std::vector<int>> GetField() const;
	bool GetHasMoveMino() const { return hasMoveMino_; }

private:

	void SetTetriminoToField();

	std::vector<std::vector<int>> field_;
	std::array<std::pair<int, int>, 4> currentMino_{};
	const int width_;
	const int height_;
	std::pair<int, int> spawnPosition_;

	std::array<std::pair<int, int>, 4> moveMino_;
	int minoType_ = 0;

	std::unique_ptr<ParticleResource> debugSphere_ = nullptr;

	KeyManager* keyManager_ = nullptr;

	float downTimer_ = 0.0f;

	bool isDown_ = false;
	bool notAllowDown_ = false;
	bool hasMoveMino_ = false;
};
