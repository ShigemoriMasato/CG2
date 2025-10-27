#include "CommonData.h"

CommonData::CommonData() {
	keyManager = std::make_unique<KeyManager>();
	keyManager->Initialize();

	keyManager->SetKey(Key::Right, DIK_RIGHTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Right, XBoxController::kRight, KeyState::Hold);
	keyManager->SetStick(Key::Right, true, false, 0.5f);

	keyManager->SetKey(Key::Left, DIK_LEFTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Left, XBoxController::kLeft, KeyState::Hold);
	keyManager->SetStick(Key::Left, true, false, -0.5f);

	keyManager->SetKey(Key::Up, DIK_UPARROW, KeyState::Hold);
	keyManager->SetButton(Key::Up, XBoxController::kUp, KeyState::Hold);
	keyManager->SetStick(Key::Up, true, true, 0.5f);

	keyManager->SetKey(Key::Down, DIK_DOWNARROW, KeyState::Hold);
	keyManager->SetButton(Key::Down, XBoxController::kDown, KeyState::Hold);
	keyManager->SetStick(Key::Down, true, true, -0.5f);
	
	//================================================================================

	keyManager->SetKey(Key::HardDrop, DIK_SPACE, KeyState::Trigger);
	keyManager->SetKey(Key::HardDrop, DIK_UPARROW, KeyState::Trigger);
	keyManager->SetButton(Key::HardDrop, XBoxController::kUp, KeyState::Trigger);

	keyManager->SetKey(Key::Hold, DIK_LSHIFT, KeyState::Trigger);
	keyManager->SetKey(Key::Hold, DIK_C, KeyState::Trigger);
	keyManager->SetKey(Key::Hold, DIK_RSHIFT, KeyState::Trigger);
	keyManager->SetButton(Key::Hold, XBoxController::kLeftShoulder, KeyState::Trigger);
	keyManager->SetButton(Key::Hold, XBoxController::kLeftTrigger, KeyState::Trigger);
	keyManager->SetButton(Key::Hold, XBoxController::kRightShoulder, KeyState::Trigger);
	keyManager->SetButton(Key::Hold, XBoxController::kLeftTrigger, KeyState::Trigger);

	//================================================================================

	keyManager->SetKey(Key::LRotate, DIK_Z, KeyState::Trigger);
	keyManager->SetButton(Key::LRotate, XBoxController::kX, KeyState::Trigger);

	keyManager->SetKey(Key::RRotate, DIK_X, KeyState::Trigger);
	keyManager->SetButton(Key::RRotate, XBoxController::kY, KeyState::Trigger);

	//================================================================================

	keyManager->SetKey(Key::Correct, DIK_RETURN, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_SPACE, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_Z, KeyState::Trigger);
	keyManager->SetButton(Key::Correct, XBoxController::kA, KeyState::Trigger);
	
	keyManager->SetKey(Key::Reverse, DIK_X, KeyState::Trigger);
	keyManager->SetButton(Key::Reverse, XBoxController::kB, KeyState::Trigger);

	keyManager->SetKey(Key::Pause, DIK_ESCAPE, KeyState::Trigger);
	keyManager->SetKey(Key::Pause, DIK_F1, KeyState::Trigger);
	keyManager->SetButton(Key::Pause, XBoxController::kStart, KeyState::Trigger);
}

CommonData::~CommonData() {
	
}
