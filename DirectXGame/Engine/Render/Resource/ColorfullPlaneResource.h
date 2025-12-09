#pragma once
#include "Data/BaseResource.h"

class ColorfullPlaneResource : public BaseResource {
public:

	ColorfullPlaneResource() = default;
	~ColorfullPlaneResource() = default;

	//左上、右上、左下、右下の順番
	void Initialize(uint32_t* vertexColor);

	void DrawReady() override;

	Vector3 scale_;
	Vector3 rotation_;
	Vector3 position_;

	uint32_t color_ = 0xffffffff;

	Camera* camera_ = nullptr;

private:

	VertexData* vertexData_ = nullptr;
	Vector4* colorData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	Matrix4x4* wvpMatrix_{};
	Vector4* objectColor_{};

};
