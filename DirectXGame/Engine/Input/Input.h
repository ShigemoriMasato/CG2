#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <wrl.h>

#include "../Function/MyMath.h"

enum class XBoxController {
	kUnknown = -1,
	kA,
	kB,
	kX,
	kY,
	kUp,
	kDown,
	kLeft,
	kRight,
	kSelect,
	kStart,
	kRightShoulder,
	kRightTrigger,
	kLeftShoulder,
	kLeftTrigger,

	kButtomCount
};

class Input {
public:
	Input(HINSTANCE hInstance, HWND hwnd) : hInstance_(hInstance), hwnd_(hwnd) {}
	~Input() = default;

	void SetHWND(HWND hwnd) {
		hwnd_ = hwnd;
	}

	void Initialize();

	void Update();

	static BYTE* GetKeyState();

	static BYTE* GetPreKeyState();

	static BYTE GetKeyState(int keyCode);

	static BYTE GetPreKeyState(int keyCode);

	static Vector2 GetMouseMove();

	static float GetMouseWheel();

	static BYTE* GetMouseButtonState();

	static BYTE* GetPreMouseButtonState();

	static bool* GetXBoxButtonState();

	static bool GetXBoxButtonState(XBoxController button);

	/// <param name="type">0:右スティック	1:左スティック</param>
	static Vector2 GetXBoxStickState(int type);

private:

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	
	XINPUT_STATE xBoxState_{};
	static bool xBoxButtonFlug_[int(XBoxController::kButtomCount)];
	static Vector2 xBoxStickState_[2]; //0:右スティック 1:左スティック

	//どこからでもGetできるようにstaticにする
	static BYTE keyState[256];
	static BYTE preKeyState[256];

	static DIMOUSESTATE preMouseState;
	static DIMOUSESTATE mouseState;

	//Initializeをしてないとstatic関数を使えなくする
	static bool isInitialized_;

	HINSTANCE hInstance_ = nullptr;
	HWND hwnd_ = nullptr;

};
