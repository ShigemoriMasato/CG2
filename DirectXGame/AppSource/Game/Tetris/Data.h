#pragma once
#include <utility>
#include <array>

struct MovableMino {
	std::vector<std::pair<int, int>> positions;
	int minoType;
};
