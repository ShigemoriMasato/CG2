#pragma once
#include <cstdint>

struct EntityID {
	uint32_t id;

	bool operator==(const EntityID& other) const {
		return id == other.id;
	}
	bool operator!=(const EntityID& other) const {
		return id != other.id;
	}
	bool operator<(const EntityID& other) const {
		return id < other.id;
	}
};
