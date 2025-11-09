#pragma once
#include <string>
#include <Core/DXDevice.h>
#include <vector>
#include <Transform/Transform.h>
#include <Func/MyMath.h>
#include "ModelAnimation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class AssetsLoader;

struct ModelMaterial final {
	std::string name;
	uint32_t textureHandle;
};

struct Node {
	Matrix4x4 localMatrix = Matrix::MakeIdentity4x4();
	std::string name{};
	int nodeIndex = -1;
	std::vector<Node> children{};
};

class ModelData {
public:

	struct VertexResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_VERTEX_BUFFER_VIEW bufferView = {};
		int vertexNum = 0;
	};

	struct IndexResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
		D3D12_INDEX_BUFFER_VIEW bufferView = {};
		int indexNum = 0;
	};

	ModelData() = default;
	~ModelData() = default;

	virtual void LoadModel(const std::string& directoryPath, const std::string& filename, AssetsLoader* assetsManager, DXDevice* device);

	Node GetParentNode() { return rootNode_; }
	VertexResource GetVertexResource() const { return vbv_; }
	IndexResource GetIndexResource() const { return ibv_; }
	std::vector<ModelMaterial> GetMaterials() const { return material_; }
	int GetNodeCount() const { return nodeCount_; }

	Animation animation_{};

protected:

	void LoadMaterial(const aiScene* scene, std::string directoryPath, AssetsLoader* assetsManager);
	Node LoadNode(aiNode* node, const aiScene* scene);
	void CreateID3D12Resource(ID3D12Device* device);

	std::vector<ModelMaterial> material_{};
	std::vector<ModelVertexData> vertices_{};
	std::vector<VertexTexture> vertexTextures_{};
	std::vector<uint32_t> indices_{};

	VertexResource vbv_{};
	IndexResource ibv_{};

	int nodeCount_ = 0;

	Node rootNode_{};

};

