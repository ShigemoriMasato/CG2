#include "ParticleResource.h"

void ParticleResource::Initialize(ShapeType type, uint32_t instanceNum) {
#pragma region 球の設定
	int vertexCount = 0;
	int indexCount = 0;

	int vertical = 16;
	int horizontal = 32;
	float radius = 0.5f;

	float theta;
	float phi;

	float sinTheta;
	float cosTheta;

	float sinPhi;
	float cosPhi;
#pragma endregion

	std::vector<Vector3> position(1000, {});
	std::vector<Vector2> texcoord(1000, {});
	std::vector<Vector3> normal(1000, {});
	index_.resize(4000, 0);

	//Primitiveの頂点を作成
	switch (type) {
	case ShapeType::Plane:
		MakeVertex(vertex_, 4);
		MakeIndex(indices_, 6);
		position = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f}
		};
		texcoord = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f}
		};
		normal = {
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		};
		index_ = {
			0, 1, 2,
			2, 1, 3
		};

		break;

	case ShapeType::Cube:

		MakeVertex(vertex_, 8);
		MakeIndex(indices_, 36);

		position = {
			{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
		};
		texcoord = {
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{0.0f, 1.0f}, {1.0f, 1.0f},
			{1.0f, 0.0f}, {0.0f, 0.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}
		};
		normal = {
			{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
			{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
		};
		index_ = {
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

		break;

	case ShapeType::Sphere:

		MakeVertex(vertex_, 561);
		MakeIndex(indices_, 3072);

		for (int i = 0; i < vertical + 1; ++i) {
			theta = i * std::numbers::pi_v<float> / vertical; // 緯度

			sinTheta = std::sin(theta);
			cosTheta = std::cos(theta);

			for (int j = 0; j < horizontal + 1; ++j) {
				phi = j * std::numbers::pi_v<float> *2.0f / horizontal;

				sinPhi = std::sin(phi);
				cosPhi = std::cos(phi);

				// 球の頂点座標を計算
				position[vertexCount].x = radius * cosPhi * sinTheta;
				position[vertexCount].y = radius * cosTheta;
				position[vertexCount].z = radius * sinPhi * sinTheta;

				texcoord[vertexCount].x = static_cast<float>(j) / horizontal;
				texcoord[vertexCount].y = static_cast<float>(i) / vertical;

				normal[vertexCount].x = cosPhi * sinTheta;
				normal[vertexCount].y = cosTheta;
				normal[vertexCount].z = sinPhi * sinTheta;

				++vertexCount;
			}
		}

		for (int i = 0; i < vertical; ++i) {
			for (int j = 0; j < horizontal; ++j) {

				index_[indexCount++] = i * (horizontal + 1) + j;
				index_[indexCount++] = i * (horizontal + 1) + j + 1;
				index_[indexCount++] = (i + 1) * (horizontal + 1) + j;

				index_[indexCount++] = i * (horizontal + 1) + j + 1;
				index_[indexCount++] = (i + 1) * (horizontal + 1) + j + 1;
				index_[indexCount++] = (i + 1) * (horizontal + 1) + j;

			}
		}

		break;
	}

	for(uint32_t i = 0; i < vertexNum_; ++i) {
		vertex_[i].position = { position[i].x, position[i].y, position[i].z, 1.0f };
		vertex_[i].texcoord = texcoord[i];
		vertex_[i].normal = normal[i];
	}

	std::memcpy(indices_, index_.data(), sizeof(uint32_t)* indexNum_);

	instanceNum_ = instanceNum;

	position_.resize(instanceNum_);
	rotate_.resize(instanceNum_);
	scale_.resize(instanceNum_, { 1.0f, 1.0f, 1.0f });
	textureIndex_.resize(instanceNum_);
	color_.resize(instanceNum_, 0xffffffff);

	MakeSRV(particleData_, instanceNum_);
	MakeUAV();

	psoConfig_.ps = "Particle.PS.hlsl";
	psoConfig_.vs = "Particle.VS.hlsl";
	psoConfig_.inputLayoutID = InputLayoutID::Default;
	psoConfig_.rootID = RootSignatureID::Particle;
}

void ParticleResource::DrawReady() {

	//ParticleData
	Matrix4x4 vp = camera_->GetVPMatrix();
	for (uint32_t i = 0; i < instanceNum_; ++i) {
		particleData_[i].world = MakeAffineMatrix(scale_[i], rotate_[i], position_[i]);
		particleData_[i].wvp = vp;
		particleData_[i].color = ConvertColor(color_[i]);
		particleData_[i].textureIndex = textureIndex_[i].id;
	}
}
