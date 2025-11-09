#include "DrawResource.h"
#include <Core/DXCommonFunction.h>
#include <Func/MyMath.h>
#include <numbers>

using namespace Matrix;

DrawResource::~DrawResource() {
}

void DrawResource::Initialize(uint32_t vertexNum, uint32_t indexNum) {
	psoConfig_ = PSOConfig{};

	auto device = dxDevice_->GetDevice();

	MakeVertex(vertex_, vertexNum);
	MakeVertex(vertexTexture_, vertexNum);

	if (indexNum > 0)
		MakeIndex(indices_, indexNum);

	localPos_.resize(vertexNum);
	texcoord_.resize(vertexNum);
	normal_.resize(vertexNum);
	index_.resize(indexNum);

	MakeCBV(matrix_);
	MakeCBV(material_);
	MakeCBV(light_);
	MakeUAV();

	matrix_->world = Matrix::MakeIdentity4x4();
	matrix_->wvp = Matrix::MakeIdentity4x4();

	vertexNum_ = vertexNum;
	indexNum_ = indexNum;

	textureIndex_ = 0;
}

void DrawResource::Initialize(ShapeType type) {
#pragma region 球の設定
	int vertexCount = 0;
	int indexCount = 0;

	int vertical = 16;
	int horizontal = 32;
	float radius = 1.0f;

	float theta;
	float phi;

	float sinTheta;
	float cosTheta;

	float sinPhi;
	float cosPhi;
#pragma endregion

	switch (type) {
	case ShapeType::Plane:
		Initialize(4, 6);
		localPos_ = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f}
		};
		texcoord_ = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f}
		};
		normal_ = {
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

		Initialize(8, 36);

		localPos_ = {
			{-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
			{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
			{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}
		};
		texcoord_ = {
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{0.0f, 1.0f}, {1.0f, 1.0f},
			{1.0f, 0.0f}, {0.0f, 0.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}
		};
		normal_ = {
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
		Initialize(561, 3072);

		for (int i = 0; i < vertical + 1; ++i) {
			theta = i * std::numbers::pi_v<float> / vertical; // 緯度

			sinTheta = std::sin(theta);
			cosTheta = std::cos(theta);

			for (int j = 0; j < horizontal + 1; ++j) {
				phi = j * std::numbers::pi_v<float> * 2.0f / horizontal;

				sinPhi = std::sin(phi);
				cosPhi = std::cos(phi);

				// 球の頂点座標を計算
				localPos_[vertexCount].x = radius * cosPhi * sinTheta;
				localPos_[vertexCount].y = radius * cosTheta;
				localPos_[vertexCount].z = radius * sinPhi * sinTheta;

				texcoord_[vertexCount].x = static_cast<float>(j) / horizontal;
				texcoord_[vertexCount].y = static_cast<float>(i) / vertical;

				normal_[vertexCount].x = cosPhi * sinTheta;
				normal_[vertexCount].y = cosTheta;
				normal_[vertexCount].z = sinPhi * sinTheta;

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
}

void DrawResource::Initialize(AssetsID modelID) {

	SetModelData(modelID);

	localPos_.resize(vertexNum_);
	texcoord_.resize(vertexNum_);
	normal_.resize(vertexNum_);
	index_.resize(indexNum_);

	MakeCBV(matrix_);
	MakeCBV(material_);
	MakeCBV(light_);
	MakeUAV();

	matrix_->world = Matrix::MakeIdentity4x4();
	matrix_->wvp = Matrix::MakeIdentity4x4();

	textureIndex_ = 0;

}

void DrawResource::DrawReady() {
	//InputLayout

	for (uint32_t i = 0; i < vertexNum_; ++i) {
		vertex_[i].position = { localPos_[i].x, localPos_[i].y, localPos_[i].z, 1.0f };
		vertex_[i].texcoord = texcoord_[i];
		vertex_[i].normal = normal_[i];
	}

	//IndexBuffer
	if (indexNum_ > 0) {
		index_.resize(indexNum_);
		std::memcpy(indices_, index_.data(), sizeof(uint32_t) * indexNum_);
	}

	//VertexTexture
	for(uint32_t i = 0; i < vertexNum_; ++i) {
		vertexTexture_[i].textureIndex = textureIndex_;
	}

	//Material
	material_->color = ConvertColor(color_);

	material_->uvTransform = MakeUVMatrix(textureScale_, textureRotate_, texturePos_);

	//Matrix
	Matrix4x4 worldMat = MakeAffineMatrix(scale_, rotate_, position_);

	//初期化
	matrix_->world = Matrix::MakeIdentity4x4();

	//親の数だけかける
	for (const auto& mat : parentMatrices_) {
		matrix_->world *= mat;
	}
	//親行列クリア
	parentMatrices_.clear();

	matrix_->world *= worldMat;

	if (camera_) {
		matrix_->wvp = matrix_->world * camera_->GetVPMatrix();
	}

	light_->enableLighting = static_cast<int32_t>(enableLighting_);

	//Lighting
	if (enableLighting_) {
		light_->color = ConvertColor(lightColor_);
		light_->intensity = lightIntensity_;
		lightDirection_ = lightDirection_.Normalize();
		light_->direction = lightDirection_;
	}
}

void DrawResource::AddParentMatrix(const Matrix4x4& parentMatrix) {
	parentMatrices_.push_back(parentMatrix);
}

ID3D12Resource* DrawResource::GetParticleDataResource() const {
	if (matrixResource) {
		return matrixResource.Get();
	}
	return nullptr;
}
