#pragma once
#include <vector>

class Field {
public:

	Field() = default;
	~Field() = default;

	void Initialize();
	void Update();
	void Draw();

private:

	std::vector<std::vector<int>> field_;
	cosnt int width_ = 10;
	const int height_ = 20;

};
