#include "EngineTerminal.h"
#include <Scene/TitleScene.h>
#include <Scene/Engine/ShaderEditScene.h>

EngineTerminal::EngineTerminal(BootMode mode) {
	mode_ = mode;
}

EngineTerminal::~EngineTerminal() {
	ImGuiOperator::Finalize();
}

bool EngineTerminal::IsLoop() {
	while (msg.message != WM_QUIT) {

		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//メッセージがなければ処理を始める
			return true;
		}

	}
	//ウィンドウのxボタンが押されたらfalseを返す
	return false;
}

void EngineTerminal::Initialize(int32_t windowWidth, int32_t windowHeight) {
	dxDevice_ = std::make_unique<DXDevice>(windowWidth, windowHeight);

	//WindowProc
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc =
		[this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {

		switch (msg) {
		case WM_DESTROY:

			PostQuitMessage(0);

			return 0;

		case WM_KEYDOWN:

			//ESCで終了
			if (wparam == VK_ESCAPE) {
				PostQuitMessage(0);
				return 0;
			}

			break;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
		};

	dxDevice_->SetWindowProc(windowProc);

	dxDevice_->Initialize();

	render_ = std::make_unique<Render>(dxDevice_.get());
	srvManager_ = std::make_unique<SRVManager>(dxDevice_.get(), 256);

	BaseResource::StaticInitialize(dxDevice_.get(), srvManager_.get());
	ParticleResource::StaticInitialize(dxDevice_.get(), srvManager_.get());
	ModelResource::StaticInitialize(dxDevice_.get(), srvManager_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get(), render_->GetCommandList(), srvManager_.get());
	modelManager_ = std::make_unique<ModelManager>(textureManager_.get(), dxDevice_.get());
	offScreenManager_ = std::make_unique<OffScreenManager>();
	offScreenManager_->Initialize(dxDevice_.get(), render_->GetCommandList(), srvManager_.get());
	input_ = std::make_unique<Input>(dxDevice_->GetWndClass().hInstance, dxDevice_->GetHwnd());
	input_->Initialize();
	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialize();

	render_->Initialize(textureManager_.get(), offScreenManager_.get(), srvManager_.get());

	ImGuiOperator::Initialize(dxDevice_.get(), render_.get(), srvManager_.get());

	textureManager_->LoadTexture("Assets/Texture/white1x1.png");

	switch (mode_) {
	case BootMode::Game:
		sceneManager_ = std::make_unique<SceneManager>(std::make_unique<TitleScene>(), this);
		break;
	case BootMode::ShaderEdit:
		sceneManager_ = std::make_unique<SceneManager>(std::make_unique<ShaderEditScene>(), this);
		break;
	}


	fpsObserver_ = std::make_unique<FPSObserver>(true, 60.0);
}

// =========================- MainLoop -===============================
void EngineTerminal::Run() {
	while (IsLoop()) {

		Update();

		sceneManager_->Update();

		sceneManager_->Draw();
		PostDraw();
	}
}



void EngineTerminal::Update() {
	input_->Update();
	auto windowSize = dxDevice_->GetWindowSize();
	ImGuiOperator::StartFrame(static_cast<float>(windowSize.first), static_cast<float>(windowSize.second));

	fpsObserver_->TimeAdjustment();
}

void EngineTerminal::PreDraw() {
}

void EngineTerminal::PostDraw() {
	render_->PostDraw(imgui_.get());
}
