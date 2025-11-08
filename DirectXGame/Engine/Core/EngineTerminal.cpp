#include "EngineTerminal.h"
#include <Common/InitializeScene/InitializeScene.h>
#include <Scene/SceneManager.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EngineTerminal::EngineTerminal(BootMode mode) {
	mode_ = mode;
}

EngineTerminal::~EngineTerminal() {
	ImGuiOperator::Finalize();
}

bool EngineTerminal::IsLoop() {
	if (sceneManager_->commonData_->isPushClose_) {
		return false;
	}

	while (true) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//メッセージがなければ処理を始める
			return true;
		}
	}
}

void EngineTerminal::Initialize(int32_t windowWidth, int32_t windowHeight) {
	dxDevice_ = std::make_unique<DXDevice>(windowWidth, windowHeight);
	dxDevice_->Initialize();

	dxDevice_->SetWindowProc([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {

		//imguiのウィンドウプロシージャを呼ぶ
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
			return true;
		}

		switch (msg) {
		case WM_CLOSE:
			sceneManager_->commonData_->isPushClose_ = true;
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);

	});

	render_ = std::make_unique<Render>(dxDevice_.get());
	srvManager_ = std::make_unique<SRVManager>(dxDevice_.get(), 4096);

	offScreenManager_ = std::make_unique<OffScreenManager>();
	offScreenManager_->Initialize(dxDevice_.get(), srvManager_.get());
	input_ = std::make_unique<Input>(dxDevice_->GetWndClass().hInstance, dxDevice_->GetHwnd());
	input_->Initialize();

	render_->Initialize(offScreenManager_.get(), srvManager_.get());

	ImGuiOperator::Initialize(dxDevice_.get(), render_.get(), srvManager_.get());

	fpsObserver_ = std::make_unique<FPSObserver>(true, 60.0);

	assetsLoader_ = std::make_unique<AssetsLoader>();
	assetsLoader_->Initialize(dxDevice_.get(), srvManager_.get(), render_->GetCommandList());

	BaseResource::StaticInitialize(dxDevice_.get(), assetsLoader_.get(), offScreenManager_.get(), srvManager_.get());

	switch (mode_) {
	case BootMode::Game:
		sceneManager_ = std::make_unique<SceneManager>(std::make_unique<InitializeScene>(), this);
		break;
	}
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
