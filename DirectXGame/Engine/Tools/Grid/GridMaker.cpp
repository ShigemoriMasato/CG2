#include "GridMaker.h"
#include <Math/MyMath.h>
#include <cmath>

using namespace MyMath;

GridLine::GridLine(Camera* camera, LineType type, Vector3 center) {

	center_ = center;

	if (type == LineType::kHorizontal) {
		
		start_ = { center.x, center.y, center.z + GRID_LINE_LENGTH / 2 };
		end_ = { center.x, center.y, center.z - GRID_LINE_LENGTH / 2 };

		if (static_cast<int>(center.x) % 10 == 0) {

			shapeType_ = ShapeType::kThickLine;
			thickness_ = 0.3f;

			if (static_cast<int>(center.x) == 0) {
				thickness_ = 0.3f;
			}
		}

	} else {

		start_ = { center.x + GRID_LINE_LENGTH / 2, center.y, center.z };
		end_ = { center.x - GRID_LINE_LENGTH / 2, center.y, center.z };

		if (static_cast<int>(center.z) % 10 == 0) {

			shapeType_ = ShapeType::kThickLine;
			thickness_ = 0.3f;

			if (static_cast<int>(center.z) == 0) {
				thickness_ = 0.3f;
			}
		}

	}

	center_ = center;

	type_ = type;
}

void GridLine::AdjustCenter(float center) {
	if (type_ == LineType::kHorizontal) {

		start_ = { center_.x, center_.y, center + GRID_LINE_LENGTH / 2 };
		end_ = { center_.x, center_.y, center - GRID_LINE_LENGTH / 2 };

	} else {

		start_ = { center + GRID_LINE_LENGTH / 2, center_.y, center_.z };
		end_ = { center - GRID_LINE_LENGTH / 2, center_.y, center_.z };

	}
}

GridMaker::GridMaker(Camera* camera) {
	if (dynamic_cast<DebugCamera*>(camera)) {
		debugCamera_ = dynamic_cast<DebugCamera*>(camera);
	}

	camera_ = camera;

	Initialize();
	lineResource_ = std::make_unique<DrawResource>();
	lineResource_->Initialize(724, 0, true);
	lineResource_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	lineResource_->color_ = 0x808080ff;
	lineResource_->camera_ = camera_;

	thickLineResource_ = std::make_unique<DrawResource>();
	thickLineResource_->Initialize(252, 0, true);
	thickLineResource_->color_ = 0xff8000ff;
	thickLineResource_->camera_ = camera_;
}

void GridMaker::Initialize() {
	Transform transform{};

	for (int i = 0; i < 2; ++i) {

		lines_[i].clear(); // 既存のラインをクリア

		for (int j = 0; j < kGridCount; ++j) {
			// i == 0: 縦線, i == 1: 横線
			Vector3 center = nowMid;
			if (i == 0) {
				center.x = center.x - (kGridCount * gridSize_.x) / 2 + float(j * gridSize_.x);
			} else {
				center.z = center.z - (kGridCount * gridSize_.y) / 2 + float(j * gridSize_.y);
			}

			lines_[i].push_back(std::make_unique<GridLine>(camera_, i == 0 ? GridLine::kHorizontal : GridLine::kVertical, center));

		}
	}
}

void GridMaker::Update() {
	Vector3 mid{};
	if(debugCamera_) {
		mid = debugCamera_->GetCenter();
	} else {
		mid = camera_->GetPosition();
	}
	
	bool isAdjust = false;

	// 縦線の調整
	while (!isAdjust) {
		// グリッド一個分ずれているか
		if(std::fabsf(nowMid.x - mid.x) >= gridSize_.x) {

			// グリッドのずれてる方向を確認
			if (nowMid.x - mid.x < 0) {

				// 右にずれているので、左にある線を削除して、右に追加する
				lines_[0].erase(lines_[0].begin());

				Vector3 center = nowMid;
				center.x = center.x + (kGridCount * gridSize_.x) / 2;
				lines_[0].push_back(std::make_unique<GridLine>(camera_, GridLine::kHorizontal , center));

				// ずらした分だけnowMidを更新
				nowMid.x += gridSize_.x;

			} else {

				// 左にずれているので、右にある線を削除して、左に追加する
				lines_[0].pop_back();

				Vector3 center = nowMid;
				center.x = center.x - (kGridCount * gridSize_.x) / 2;
				lines_[0].insert(lines_[0].begin(), std::make_unique<GridLine>(camera_, GridLine::kHorizontal, center));

				// ずらした分だけnowMidを更新
				nowMid.x -= gridSize_.x;
			}

		} else {
			isAdjust = true;
		}
	}

	isAdjust = false;

	// 横線の調整
	while (!isAdjust) {
		// グリッド一個分ずれているか
		if(std::fabsf(nowMid.z - mid.z) >= gridSize_.y) {

			// グリッドのずれてる方向を確認
			if (nowMid.z - mid.z < 0) {

				// 上にずれているので、下にある線を削除して、上に追加する
				lines_[1].erase(lines_[1].begin());

				Vector3 center = nowMid;
				center.z = center.z + (kGridCount * gridSize_.y) / 2;
				lines_[1].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

				// ずらした分だけnowMidを更新
				nowMid.z += gridSize_.y;
			} else {

				// 下にずれているので、上にある線を削除して、下に追加する
				lines_[1].pop_back();

				Vector3 center = nowMid;
				center.z = center.z - (kGridCount * gridSize_.y) / 2;
				lines_[1].insert(lines_[1].begin(), std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

				// ずらした分だけnowMidを更新
				nowMid.z -= gridSize_.y;
			}

		} else {
			isAdjust = true;
		}

	}

	for (int i = 0; i < kGridCount; ++i) {
		lines_[0][i]->AdjustCenter(mid.z);
		lines_[1][i]->AdjustCenter(mid.x);
	}

	//以下、エンジン固有の処理。今回の場合、DXResourceに各種情報を登録する
	uint32_t lineCount = 0;
	uint32_t thickLineCount = 0;
	int vertexIndex[] = { 0, 1, 2, 1, 2, 3 };
	for (int i = 0; i < 2; ++i) {
		for (auto& line : lines_[i]) {
			if (line->GetShapeType() == GridLine::ShapeType::kLine) {
				lineResource_->localPos_[lineCount++] = line->GetStart();
				lineResource_->localPos_[lineCount++] = line->GetEnd();
			} else {

				Vector3 dir = (line->GetStart() - line->GetEnd()).Normalize();				// 線の方向
				Vector3 mid = (line->GetStart() + line->GetEnd()) * 0.5f;					// 中心点
				Vector3 view = (camera_->GetPosition() - mid).Normalize();					// カメラ方向
				Vector3 normal = cross(dir, view).Normalize();								// 線とカメラに垂直な方向
				Vector3 offset = normal * (line->GetThickness() / 2.0f);					// 太さ分のオフセット

				Vector3 pos[4] = { line->GetStart() + offset, line->GetStart() - offset, line->GetEnd() + offset, line->GetEnd() - offset };

				for (int i = 0; i < 6; ++i) {
					thickLineResource_->localPos_[thickLineCount++] = pos[vertexIndex[i]];
				}
			}
		}
	}
}

void GridMaker::Draw(Render* render) {
	render->Draw(thickLineResource_.get());
	render->Draw(lineResource_.get());
}
