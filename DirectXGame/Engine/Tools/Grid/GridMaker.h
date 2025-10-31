#pragma once
#include <Camera/DebugCamera.h>
#include <memory>
#include <vector>
#include <array>
#include <Render/Render.h>
#define GRID_LINE_LENGTH 200.0f

class GridLine {
public:

	enum class ShapeType {
		kLine,
		kThickLine
	};

	enum LineType {
		kHorizontal,
		kVertical
	};

	GridLine(Camera* camera, LineType type, Vector3 center);
	~GridLine() = default;

	void AdjustCenter(float center);
	ShapeType GetShapeType() const { return shapeType_; }
	Vector3 GetStart() const { return start_; }
	Vector3 GetEnd() const { return end_; }
	float GetThickness() const { return thickness_; }

private:

	LineType type_ = kHorizontal;
	Vector3 center_{};

	ShapeType shapeType_ = ShapeType::kLine;

	Vector3 start_{};
	Vector3 end_{};
	float thickness_ = 1.0f;

};

class GridMaker {
public:

	GridMaker(Camera* camera);
	~GridMaker() = default;

	void Initialize();
	
	void Update();

	void Draw(Render* render);

private:

	std::array<std::vector<std::unique_ptr<GridLine>>, 2> lines_{};
	static const int kGridCount = static_cast<int>(GRID_LINE_LENGTH);		//Gridが生成される数
	const Vector2 gridSize_{ 1.0f, 1.0f }; //Gridで囲う空間のサイズ
	Camera* camera_;
	DebugCamera* debugCamera_ = nullptr;

	std::unique_ptr<DrawResource> lineResource_ = nullptr;
	std::unique_ptr<DrawResource> thickLineResource_ = nullptr;

	Vector3 nowMid = { 0.0f, 0.0f, 0.0f };

};

