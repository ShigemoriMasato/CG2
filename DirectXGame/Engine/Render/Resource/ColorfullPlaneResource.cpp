#include "ColorfullPlaneResource.h"
#include <Camera/Camera.h>

void ColorfullPlaneResource::Initialize(uint32_t* vertexColor) {
	BaseResource::Initialize();
	
	MakeVertex(vertexData_, 4);
	MakeVertex(colorData_, 4);

	MakeIndex(indexData_, 6);

	std::vector<Vector3> pos = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f}
	};
	std::vector<Vector2> texcoord_ = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f}
	};
	std::vector<Vector3> normal_ = {
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}
	};
	std::vector<int> index_ = {
		0, 1, 2,
		2, 1, 3
	};

	for (int i = 0; i < 4; ++i) {
		vertexData_[i].position = { pos[i].x, pos[i].y, pos[i].z, 1.0f };
		vertexData_[i].texcoord = texcoord_[i];
		vertexData_[i].normal = normal_[i];
		colorData_[i] = ConvertColor(vertexColor[i]);
	}
	std::memcpy(indexData_, index_.data(), sizeof(uint32_t) * 6);


	MakeCBV(wvpMatrix_);
	MakeCBV(objectColor_);
	
	psoConfig_.vs = "Game/ColorfullPlane.VS.hlsl";
	psoConfig_.ps = "Game/ColorfullPlane.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::VerC1_PixC1;
	psoConfig_.inputLayoutID = InputLayoutID::Color;
}

void ColorfullPlaneResource::DrawReady() {
	*wvpMatrix_ = Matrix::MakeAffineMatrix(scale_, rotation_, position_) * camera_->GetVPMatrix();
	*objectColor_ = ConvertColor(color_);
}
