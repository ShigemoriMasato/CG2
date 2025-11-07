#include "ResourceGenerator.h"

ID3D12Device* ResourceGenerator::device_ = nullptr;
SRVManager* ResourceGenerator::srvManager_ = nullptr;
AssetsLoader* ResourceGenerator::assetsLoader_ = nullptr;
RootSignatureShelf* ResourceGenerator::rsShelf_ = nullptr;
D3D12_GPU_DESCRIPTOR_HANDLE ResourceGenerator::initializeTextureHandle_ = {};

namespace {
	std::map<Primitive, std::pair<std::vector<VertexData>, std::vector<uint32_t>>> primitiveVertexData;
}

void ResourceGenerator::StaticInitialize(ID3D12Device* device, SRVManager* srv, AssetsLoader* assets) {
	device_ = device;
	srvManager_ = srv;
	assetsLoader_ = assets;

	//Primitiveデータの作成
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

	//Triangle
	vertices.resize(3);
	vertices = {
		{{0.0f, 0.5f, 0.0f} ,{0.5f, 0.0f},{0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, 0.0f},{0.0f, 1.0f},{0.0f, 0.0f, -1.0f}},
		{{0.5f, -0.5f, 0.0f} ,{1.0f, 1.0f},{0.0f, 0.0f, -1.0f}}
	};

	primitiveVertexData[Primitive::Triangle] = { vertices, {} };

	//Plane
	vertices.resize(4);
	indices.resize(6);
	vertices = {
		{{-0.5f, 0.5f, 0.0f} ,{0.0f, 0.0f},{0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}  ,{1.0f, 0.0f},{0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.0f},{0.0f, 1.0f},{0.0f, 1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f} ,{1.0f, 1.0f},{0.0f, 1.0f, 0.0f}}
	};
	indices = {
		0, 1, 2,
		2, 1, 3
	};

	primitiveVertexData[Primitive::Plane] = { vertices, indices };

	//Cube
	vertices.resize(8);
	indices.resize(36);

	vertices = {
		{{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{0.5f, 0.5f, -0.5f},   {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, 0.5f, 0.5f},   {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
		{{0.5f, 0.5f, 0.5f},    {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
		{{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
		{{0.5f, -0.5f, 0.5f},   {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }
	};

	indices = {
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

	primitiveVertexData[Primitive::Cube] = { vertices, indices };

	//Sphere
	vertices.clear();
	indices.clear();

	int vertical = 8;
	int horizontal = 16;
	float radius = 0.5f;

	float theta;
	float phi;

	float sinTheta;
	float cosTheta;

	float sinPhi;
	float cosPhi;

	for (int i = 0; i < vertical + 1; ++i) {
		theta = i * std::numbers::pi_v<float> / vertical; // 緯度

		sinTheta = std::sin(theta);
		cosTheta = std::cos(theta);

		for (int j = 0; j < horizontal + 1; ++j) {
			phi = j * std::numbers::pi_v<float> *2.0f / horizontal;

			sinPhi = std::sin(phi);
			cosPhi = std::cos(phi);

			// 球の頂点座標を計算
			vertices.push_back({
				{radius * cosPhi * sinTheta, radius * cosTheta, radius * sinPhi * sinTheta},
				  {static_cast<float>(j) / horizontal, static_cast<float>(i) / vertical},
				  {cosPhi * sinTheta, cosTheta,  sinPhi * sinTheta}
				});
		}
	}

	for (int i = 0; i < vertical; ++i) {
		for (int j = 0; j < horizontal; ++j) {

			indices.push_back(i * (horizontal + 1) + j);
			indices.push_back(i * (horizontal + 1) + j + 1);
			indices.push_back((i + 1) * (horizontal + 1) + j);

			indices.push_back(i * (horizontal + 1) + j + 1);
			indices.push_back((i + 1) * (horizontal + 1) + j + 1);
			indices.push_back((i + 1) * (horizontal + 1) + j);
		}
	}

	primitiveVertexData[Primitive::Sphere] = { vertices, indices };
	auto white1x1 = assets->Load("Assets/Texture/white1x1.png");
	initializeTextureHandle_ = assets->GetTextureData(white1x1)->GetTextureGPUHandle();
}

ResourceGenerator::ResourceGenerator() {
}

ResourceGenerator::~ResourceGenerator() {
}

void ResourceGenerator::Initialize() {
	resources_.clear();
}

void ResourceGenerator::SetVertex(Primitive type) {
	auto it = primitiveVertexData.find(type);
	if (it == primitiveVertexData.end()) return;

	const auto& [vertices, indices] = it->second;

	vertices_ = vertices;
	indices_ = indices;
}

void ResourceGenerator::SetVertex(AssetsID modelID) {
	auto modelData = assetsLoader_->GetModelData(modelID);
	if (!modelData) return;

	//todo モデルは今度
}

void ResourceGenerator::AddUAV(ShaderType type) {
	//初期値White1x1で登録
	uavAddresses_.push_back({ initializeTextureHandle_, type });
}

void ResourceGenerator::Create() {

	//頂点バッファの作成
	UINT64 vertexBufferSize = sizeof(VertexData) * vertices_.size();
	vertexResource_.Attach(CreateBufferResource(device_, vertexBufferSize));

	//頂点データ転送
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, (void**)&vertexData);
	std::memcpy(vertexData, vertices_.data(), vertexBufferSize);

	//頂点バッファビューの作成
	vbv_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(vertexBufferSize);
	vbv_.StrideInBytes = sizeof(VertexData);

	//インデックスバッファの作成
	if (!indices_.empty()) {
		UINT64 indexBufferSize = sizeof(uint32_t) * indices_.size();
		indexResource_.Attach(CreateBufferResource(device_, indexBufferSize));

		//インデックスデータ転送
		uint32_t* indexData = nullptr;
		indexResource_->Map(0, nullptr, (void**)&indexData);
		std::memcpy(indexData, indices_.data(), indexBufferSize);
		indexResource_->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibv_.BufferLocation = indexResource_->GetGPUVirtualAddress();
		ibv_.SizeInBytes = static_cast<UINT>(indexBufferSize);
		ibv_.Format = DXGI_FORMAT_R32_UINT;
	}

	//RootSignatureの作成
	psoConfig_.rootID = rsShelf_->CreateRootSignature(device_, cbvAddresses_, srvAddresses_, uavAddresses_);
}
