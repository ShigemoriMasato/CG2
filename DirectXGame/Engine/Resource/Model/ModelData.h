#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Transform/Transform.h>
#include <Resource/Texture/TextureManager.h>
#include <Math/MyMath.h>
#include <Render/Resource/DrawResource.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct ModelMaterial final {
	std::string name;
	int textureHandle = -1;
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
		std::shared_ptr<D3D12_VERTEX_BUFFER_VIEW> bufferView = nullptr;
		int vertexNum = 0;
	};

	struct IndexResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
		std::shared_ptr<D3D12_INDEX_BUFFER_VIEW> bufferView = nullptr;
		int indexNum = 0;
	};

	ModelData() = default;
	~ModelData() = default;

	virtual void LoadModel(const std::string& directoryPath, const std::string& filename, TextureManager* textureManager, DXDevice* device);

	Node GetParentNode() { return rootNode_; }
	std::unordered_map<std::string, VertexResource> GetVertexResource() const { return vertexBufferViews_; }
	std::unordered_map<std::string, IndexResource> GetIndexResource() const { return indexBufferViews_; }
	std::vector<ModelMaterial> GetMaterials() const { return material_; }
	int GetNodeCount() const { return nodeCount_; }

protected:

	void LoadMaterial(const aiScene* scene, std::string directoryPath, TextureManager* textureManager);
	Node LoadNode(aiNode* node, const aiScene* scene);
	void CreateID3D12Resource(ID3D12Device* device);

	std::vector<ModelMaterial> material_{};
	std::unordered_map<std::string, std::vector<ModelVertexData>> vertices_{};
	std::unordered_map<std::string, std::vector<uint32_t>> indices_{};

	std::unordered_map<std::string, VertexResource> vertexBufferViews_{};
	std::unordered_map<std::string, IndexResource> indexBufferViews_{};

	int nodeCount_ = 0;

	Node rootNode_{};

};

