#include "BlockResource.h"

using namespace Matrix;

void BlockResource::Initialize(uint32_t instance) {

	//頂点の確定
	VertexData* verPtr = nullptr;
	MakeVertex(verPtr, 24);
	uint32_t* indPtr = nullptr;
	MakeIndex(indPtr, 36);

	std::vector<Vector3> position = {
		{ 0.5f,  0.5f,  0.5f },
		{-0.5f,  0.5f,  0.5f },
		{-0.5f,  0.5f, -0.5f },
		{ 0.5f,  0.5f, -0.5f },

		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f,  0.5f, -0.5f },
		{-0.5f,  0.5f, -0.5f },
		{-0.5f, -0.5f, -0.5f },

		{-0.5f, -0.5f, -0.5f },
		{-0.5f,  0.5f, -0.5f },
		{-0.5f,  0.5f,  0.5f },
		{-0.5f, -0.5f,  0.5f },

		{-0.5f, -0.5f,  0.5f },
		{ 0.5f, -0.5f,  0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{-0.5f, -0.5f, -0.5f },

		{ 0.5f, -0.5f,  0.5f },
		{ 0.5f,  0.5f,  0.5f },
		{ 0.5f,  0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },

		{-0.5f, -0.5f,  0.5f },
		{-0.5f,  0.5f,  0.5f },
		{ 0.5f,  0.5f,  0.5f },
		{ 0.5f, -0.5f,  0.5f },
	};
	std::vector<Vector2> texcoord = {
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
	};
	std::vector<Vector3> normal = {
		// +Y 面
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},

		// -Z 面
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},

		// -X 面
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},

		// -Y 面
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},

		// +X 面
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},

		// +Z 面
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
	};

	std::vector<uint32_t> index_ = {
		3, 2, 1,
		3, 1, 0,

		7, 6, 5,
		7, 5, 4,

		11, 10, 9,
		11, 9, 8,

		15, 14, 13,
		15, 13, 12,

		19, 18, 17,
		19, 17, 16,

		23, 22, 21,
		23, 21, 20
	};

	for (uint32_t i = 0; i < vertexNum_; ++i) {
		verPtr[i].position = { position[i].x, position[i].y, position[i].z, 1.0f };
		verPtr[i].texcoord = texcoord[i];
		verPtr[i].normal = normal[i];
	}
	std::memcpy(indPtr, index_.data(), sizeof(uint32_t) * indexNum_);

	instanceNum_ = instance;

	MakeSRV(blockData_, instanceNum_);

	scale_.resize(instanceNum_, { 1.0f, 1.0f, 1.0f });
	rotate_.resize(instanceNum_, {});
	afterRotate_.resize(instanceNum_, {});
	position_.resize(instanceNum_, {});
	color_.resize(instanceNum_, 0);
	outlineColor_.resize(instanceNum_, 0);

	psoConfig_.vs = "Game/Block.VS.hlsl";
	psoConfig_.ps = "Game/Block.PS.hlsl";
	psoConfig_.rootID = RootSignatureID::VerS1;
}

void BlockResource::MakePointLight(uint32_t num) {
	MakeSRV(pointLights_, num);

	psoConfig_.ps = "Game/PointLightBlock.PS.hlsl";
	psoConfig_.vs = "Game/PointLightBlock.VS.hlsl";
	psoConfig_.rootID = RootSignatureID::VerS1_PixS1;

	pointLightData_.resize(num);
}

void BlockResource::MakeDirectionalAndPointLight(uint32_t directional, uint32_t point) {
	MakeSRV(pointLights_, point);
	MakeSRV(directionalLights_, directional);
	psoConfig_.ps = "Game/LightingBlock.PS.hlsl";
	psoConfig_.vs = "Game/PointLightBlock.VS.hlsl";
	psoConfig_.rootID = RootSignatureID::VerS1_PixS2;
	pointLightData_.resize(point);
	directionalLightData_.resize(directional);
}

void BlockResource::DrawReady() {
	for(uint32_t i = 0; i < instanceNum_; ++i) {
		Matrix4x4 worldMat = MakeAffineMatrix(scale_[i], rotate_[i], {}) * MakeRotationMatrix(afterRotate_[i]) * MakeTranslationMatrix(position_[i]);
		Matrix4x4 vpMat = camera_->GetVPMatrix();
		blockData_[i].world = worldMat;
		blockData_[i].vp = vpMat;
		blockData_[i].color = color_[i];
		blockData_[i].outlineColor = outlineColor_[i];
	}

	for(int i = 0; i < pointLightData_.size(); ++i) {
		pointLights_[i] = pointLightData_[i];
	}

	for(int i = 0; i < directionalLightData_.size(); ++i) {
		directionalLights_[i] = directionalLightData_[i];
	}
}
