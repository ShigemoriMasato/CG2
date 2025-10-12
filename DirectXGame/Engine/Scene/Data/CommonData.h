#pragma once
#include <Tools/KeyConfig/KeyManager.h>

class CommonData {
public:

	CommonData();
	~CommonData();

	std::unique_ptr<KeyManager> keyManager_ = nullptr;

private:

};
