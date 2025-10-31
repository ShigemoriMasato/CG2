#include <Core/EngineTerminal.h>

struct SceneTools {
	ModelManager* modelManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	Render* render_ = nullptr;
	Input* input_ = nullptr;
	AudioManager* audio_ = nullptr;
	OffScreenManager* offScreenManager_ = nullptr;
	FPSObserver* fpsObserver_ = nullptr;
};
