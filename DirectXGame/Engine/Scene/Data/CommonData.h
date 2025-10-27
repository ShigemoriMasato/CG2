#pragma once
#include <Tools/KeyConfig/KeyManager.h>
#include <random>

class CommonData {
public:

	CommonData();
	~CommonData();

	std::unique_ptr<KeyManager> keyManager = nullptr;
	std::random_device rd;

private:

};
