#pragma once
#include <Core/DXDevice.h>
#include <Render/Render.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <Assets/OffScreen/OffScreenManager.h>
#include <Assets/Audio/AudioManager.h>
#include <Input/Input.h>
#include <Render/ImGuiWrapper.h>
#include <Tools/FPS/FPSObserver.h>

class SceneManager;

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

enum class BootMode {
	Game,
	ShaderEdit,

	Count
};

class EngineTerminal {
public:

	EngineTerminal(BootMode mode);
	~EngineTerminal();
	void Initialize(int32_t windowWidth, int32_t windowHeight);

	void Run();

	DXDevice* GetDXDevice() { return dxDevice_.get(); }
	Render* GetRender() { return render_.get(); }
	TextureManager* GetTextureManager() { return textureManager_.get(); }
	ModelManager* GetModelManager() { return modelManager_.get(); }
	FPSObserver* GetFPSObserver() { return fpsObserver_.get(); }
	Input* GetInput() { return input_.get(); }
	AudioManager* GetAudioManager() { return audioManager_.get(); }
	OffScreenManager* GetOffScreenManager() { return offScreenManager_.get(); }

private:

	bool IsLoop();

	void Update();

	void PreDraw();
	void PostDraw();

	D3DResourceLeakChecker leakChecker_;
	std::unique_ptr<DXDevice> dxDevice_ = nullptr;
	std::unique_ptr<Render> render_ = nullptr;
	std::unique_ptr<SRVManager> srvManager_ = nullptr;
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	std::unique_ptr<ModelManager> modelManager_ = nullptr;
	std::unique_ptr<OffScreenManager> offScreenManager_ = nullptr;
	std::unique_ptr<Input> input_ = nullptr;
	std::unique_ptr<AudioManager> audioManager_ = nullptr;
	std::unique_ptr<ImGuiWrapper> imgui_ = nullptr;
	std::unique_ptr<FPSObserver> fpsObserver_ = nullptr;

	std::unique_ptr<SceneManager> sceneManager_ = nullptr;

	MSG msg{};
	BootMode mode_;
};

