#include <Core/EngineTerminal.h>

struct SceneTools {
	AssetsLoader* assetsLoader_ = nullptr;
	Render* render_ = nullptr;
	Input* input_ = nullptr;
	OffScreenManager* offScreenManager_ = nullptr;
	FPSObserver* fpsObserver_ = nullptr;
};
