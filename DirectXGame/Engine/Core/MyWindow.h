#pragma once
#include <Windows.h>
#include <imgui/imgui_impl_win32.h>
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

class MyWindow {
public:
	
	MyWindow(const int32_t kClientWidth, const int32_t kClientHeight);
	~MyWindow();
	HWND GetHwnd();
	WNDCLASS GetWndClass();

	void CreateWindowForApp(std::wstring windowName = L"CG", std::wstring windowClassName = L"CGClass");

	void SetWindowProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

	//hwndを統括するためにstaticを使用
	HWND hwnd_;
	WNDCLASS wc_;
	const int32_t kClientWidth_;
	const int32_t kClientHeight_;

	static std::unordered_map<HWND, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> wndProcMap_;
};
