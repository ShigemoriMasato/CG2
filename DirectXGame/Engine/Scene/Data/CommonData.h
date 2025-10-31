#pragma once
#include <Common/KeyConfig/KeyManager.h>

class CommonData {
public:

	CommonData();
	~CommonData();

	std::shared_ptr<KeyManager> keyManager_ = nullptr;

	bool isPushClose_ = false;

	int bgmPlayHandle_ = -1;

private:

};
