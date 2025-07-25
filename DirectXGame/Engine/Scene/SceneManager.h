#pragma once
#include "../Render/MyDirectX.h"
#include <memory>

class Scene;
struct CommonData;
class Input;
class Render;
class Sound;

class SceneManager final {
public:

	SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight);
	~SceneManager();
	bool IsRoop();

	void Update();
	void Draw() const;

private:

	Logger* logger = nullptr;

	std::unique_ptr<Scene> scene_ = nullptr;
	std::unique_ptr<Scene> nextScene_ = nullptr;

	std::shared_ptr<CommonData> commonData_ = nullptr;

	std::shared_ptr<MyDirectX> myDirectX_ = nullptr;
	std::unique_ptr<Render> render_ = nullptr;

	std::unique_ptr<Input> input_ = nullptr;
	std::unique_ptr<Sound> sound_ = nullptr;

	MSG msg{};
};
