#include "MyWindow.h"

std::unordered_map<HWND, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> MyWindow::wndProcMap_;

namespace {
    std::unordered_map<HWND, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> windowMap;
}

MyWindow::MyWindow(const int32_t kClientWidth, const int32_t kClientHeight) :
kClientHeight_(kClientHeight),
kClientWidth_(kClientWidth) {
    
}

MyWindow::~MyWindow() {
    //ウィンドウを破棄
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

HWND MyWindow::GetHwnd() {
	return hwnd_;
}

WNDCLASS MyWindow::GetWndClass() {
	return wc_;
}

void MyWindow::CreateWindowForApp(std::wstring windowName, std::wstring windowClassName) {
    //ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    //ウィンドウクラス名
    wc_.lpszClassName = windowClassName.c_str();
    //インスタンスハンドル
    wc_.hInstance = GetModuleHandleA(nullptr);
    //カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスの登録
    RegisterClass(&wc_);

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, kClientWidth_, kClientHeight_ };

    //クライアント領域をもとに実際のサイズにwrcに変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    //ウィンドウの作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,			//利用するクラスの名前
        windowName.c_str(),			//タイトルバーの文字
        WS_OVERLAPPEDWINDOW,		//よく見るウィンドウスタイル
        CW_USEDEFAULT,				//表示x座標
        CW_USEDEFAULT,				//表示y座標
        wrc.right - wrc.left,		//ウィンドウ幅
        wrc.bottom - wrc.top,		//ウィンドウ高さ
        nullptr,					//親ウィンドウハンドル
        nullptr,					//メニューハンドル
        wc_.hInstance,				//インスタンスハンドル
        nullptr);					//オプション

    ShowWindow(hwnd_, SW_SHOW);	    //ウィンドウを表示する

}

void MyWindow::SetWindowProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc) {
	windowMap[hwnd_] = windowProc;
}

LRESULT MyWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	auto it = windowMap.find(hwnd);
    if (it != windowMap.end())
        return windowMap[hwnd](hwnd, msg, wparam, lparam);

    //標準のメッセージ
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
