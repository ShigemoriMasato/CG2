#pragma once
#include "DrawResource.h"
#include <Resource/Model/ModelManager.h>

struct ModelDrawData {
	int textureHandle = -1;
	D3D12_VERTEX_BUFFER_VIEW* vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW* indexBufferView = {};
	int indexNum = 0;
};

/// <summary>
/// ModelDataをもとにDXResourceを生成するクラス
/// </summary>
class ModelResource {
public:

	struct NodeTransform {
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		Vector3 rotate = { 0.0f, 0.0f, 0.0f };
		Vector3 translate = { 0.0f, 0.0f, 0.0f };
		Matrix4x4 localMatrix = Matrix::MakeIdentity4x4();
		std::string name;
		std::vector<NodeTransform> children;
		int nodeIndex = -1;
	};

	static void StaticInitialize(DXDevice* device, SRVManager* srvManager) {
		dxDevice_ = device;
		srvManager_ = srvManager;
	}

	ModelResource();
	~ModelResource();

	void Initialize(ModelManager* manager, int modelHandle);

	std::unordered_map<std::string, ModelDrawData> GetModelDrawDatas() const { return modelDrawDatas_; }
	ID3D12Resource* GetMaterialResource() const { return materialResource_.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetMatrixSRVDesc() const { return matrixGPUHandle_; }

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady();

	Vector3 position_{};
	Vector3 rotate_{};
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	uint32_t color_ = 0xffffffff;

	NodeTransform node_{};

	Camera* camera_ = nullptr;

	PSOConfig psoConfig_{};

private:

	NodeTransform ConvertNodeToTransform(const Node& node);

	void DrawReadyNode(NodeTransform node, const Matrix4x4& parentMatrix);

	std::unordered_map<std::string, ModelDrawData> modelDrawDatas_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE matrixGPUHandle_ = {};

	MatrixData* matrix_ = nullptr;
	Material* material_ = nullptr;

	static DXDevice* dxDevice_;
	static SRVManager* srvManager_;
};
