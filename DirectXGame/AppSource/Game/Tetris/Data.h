#pragma once
#include <utility>
#include <array>

struct MovableMino {
	std::array<std::pair<int, int>, 4> positions;
	int minoType;
};
