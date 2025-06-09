#pragma once
#include "../Render/MyDirectX.h"

class Scene;
struct CommonData;
class Input;
class Render;
class Sound;

class SceneManager final {
public:

	SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight);
	~SceneManager();
	void Update();
	void Draw() const;
	
private:

	Scene* scene_ = nullptr;
	Scene* nextScene_ = nullptr;

	CommonData* commonData_ = nullptr;

	MyDirectX* myDirectX_ = nullptr;
	Render* renderer_ = nullptr;

	Input* input_ = nullptr;
	Sound* sound_ = nullptr;
};
