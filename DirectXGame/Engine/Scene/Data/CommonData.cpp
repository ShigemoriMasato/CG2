#include "CommonData.h"

CommonData::CommonData() {
	keyManager_ = std::make_unique<KeyManager>();
	keyManager_->Initialize();

	keyManager_->SetKey(Key::Right, DIK_D, KeyState::Hold);
	keyManager_->SetKey(Key::Right, DIK_RIGHTARROW, KeyState::Hold);
	keyManager_->SetButton(Key::Right, XBoxController::kRight, KeyState::Hold);
	keyManager_->SetStick(Key::Right, true, false, 0.5f);

	keyManager_->SetKey(Key::Left, DIK_A, KeyState::Hold);
	keyManager_->SetKey(Key::Left, DIK_LEFTARROW, KeyState::Hold);
	keyManager_->SetButton(Key::Left, XBoxController::kLeft, KeyState::Hold);
	keyManager_->SetStick(Key::Left, true, false, -0.5f);

	keyManager_->SetKey(Key::Up, DIK_W, KeyState::Hold);
	keyManager_->SetKey(Key::Up, DIK_UPARROW, KeyState::Hold);
	keyManager_->SetButton(Key::Up, XBoxController::kUp, KeyState::Hold);
	keyManager_->SetStick(Key::Up, true, true, 0.5f);

	keyManager_->SetKey(Key::Down, DIK_S, KeyState::Hold);
	keyManager_->SetKey(Key::Down, DIK_DOWNARROW, KeyState::Hold);
	keyManager_->SetButton(Key::Down, XBoxController::kDown, KeyState::Hold);
	keyManager_->SetStick(Key::Down, true, true, -0.5f);
	
	keyManager_->SetKey(Key::Action, DIK_SPACE, KeyState::Trigger);
	keyManager_->SetKey(Key::Action, DIK_Z, KeyState::Trigger);
	
	keyManager_->SetKey(Key::Correct, DIK_RETURN, KeyState::Trigger);
	keyManager_->SetKey(Key::Correct, DIK_SPACE, KeyState::Trigger);
	keyManager_->SetKey(Key::Correct, DIK_Z, KeyState::Trigger);
	keyManager_->SetButton(Key::Correct, XBoxController::kA, KeyState::Trigger);
	
	keyManager_->SetKey(Key::Reverse, DIK_ESCAPE, KeyState::Trigger);
	keyManager_->SetKey(Key::Reverse, DIK_X, KeyState::Trigger);
	keyManager_->SetButton(Key::Reverse, XBoxController::kB, KeyState::Trigger);

}

CommonData::~CommonData() {
}
