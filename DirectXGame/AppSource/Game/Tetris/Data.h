#pragma once
#include <utility>
#include <array>

struct MovableMino {
	std::vector<std::pair<int, int>> offset;
	std::pair<int, int> position;
	int minoType;
};
