#include "MyDirectX.h"
#include "Logger.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyDirectX* myDirectX = new MyDirectX(1280, 720);
	myDirectX->Initialize();

	MSG msg{};

	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//ゲームの更新処理
			myDirectX->ClearScreen();
			myDirectX->DrawTriangle();
			myDirectX->EndFrame();
		}
	}

	myDirectX->Finalize();
	delete myDirectX;

	return 0;
}
