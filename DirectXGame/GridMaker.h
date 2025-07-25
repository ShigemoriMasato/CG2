#pragma once
#include "Object/Object.h"
#include "Engine/Camera/DebugCamera.h"
#include <memory>
#include <vector>
#include <array>
#define GRID_LINE_LENGTH 200.0f

class GridLine : public Object {
public:

	enum LineType {
		kHorizontal,
		kVertical
	};

	GridLine(Camera* camera, LineType type, Vector3 center);
	~GridLine() = default;

	void Initialize() override {};
	void Update() override {};
	void AdjustCenter(float center);

private:

	LineType type_ = kHorizontal;
	Vector3 center_{};

};

class GridMaker {
public:

	GridMaker(Camera* camera, bool isDebugCamera);
	~GridMaker() = default;

	void Initialize();
	
	void Update();

	void Draw() const;

private:

	std::array<std::vector<std::unique_ptr<GridLine>>, 2> lines_{};
	static const int kGridCount = static_cast<int>(GRID_LINE_LENGTH);		//Gridが生成される数
	const Vector2 gridSize_{ 1.0f, 1.0f }; //Gridで囲う空間のサイズ
	Camera* camera_;
	DebugCamera* debugCamera_ = nullptr;

	Vector3 nowMid = { 0.0f, 0.0f, 0.0f };

};

