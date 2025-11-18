#include "BlockResource.h"

using namespace Matrix;

void BlockResource::Initialize(uint32_t instance) {

	//頂点の確定
	VertexData* verPtr = nullptr;
	MakeVertex(verPtr, 8);
	uint32_t* indPtr = nullptr;
	MakeIndex(indPtr, 36);

	std::vector<Vector3> localPos_ = {
		{-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
		{0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
		{-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
	};
	std::vector<Vector2> texcoord_ = {
		{0.0f, 0.0f}, {0.0f, 1.0f},
		{1.0f, 0.0f}, {1.0f, 1.0f},
		{1.0f, 0.0f}, {1.0f, 1.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}
	};
	std::vector<Vector3> normal_ = {
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
	};
	std::vector<uint32_t> index_ = {
		//前
		2, 3, 1,
		2, 1, 0,
		//後
		4, 5, 7,
		4, 7, 6,
		//左
		6, 7, 3,
		6, 3, 2,
		//右
		1, 5, 4,
		1, 4, 0,
		//上
		0, 4, 6,
		0, 6, 2,
		//下
		3, 7, 5,
		3, 5, 1
	};

	for (uint32_t i = 0; i < vertexNum_; ++i) {
		verPtr[i].position = { localPos_[i].x, localPos_[i].y, localPos_[i].z, 1.0f };
		verPtr[i].texcoord = texcoord_[i];
		verPtr[i].normal = normal_[i];
	}
	std::memcpy(indPtr, index_.data(), sizeof(uint32_t) * indexNum_);

	instanceNum_ = instance;

	MakeSRV(blockData_, instanceNum_);

	scale_.resize(instanceNum_, { 1.0f, 1.0f, 1.0f });
	rotate_.resize(instanceNum_, {});
	position_.resize(instanceNum_, {});
	color_.resize(instanceNum_, 0);
	outlineColor_.resize(instanceNum_, 0);

	psoConfig_.vs = "Game/Block.VS.hlsl";
	psoConfig_.ps = "Game/Block.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::Block;
}

void BlockResource::DrawReady() {
	for(uint32_t i = 0; i < instanceNum_; ++i) {
		Matrix4x4 worldMat = MakeAffineMatrix(scale_[i], rotate_[i], position_[i]);
		Matrix4x4 vpMat = camera_->GetVPMatrix();
		blockData_[i].world = worldMat;
		blockData_[i].vp = vpMat;
		blockData_[i].color = color_[i];
		blockData_[i].outlineColor = outlineColor_[i];
	}
}
