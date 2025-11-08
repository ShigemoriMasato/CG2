#pragma once
#include <Camera/Camera.h>
#include "Data/BaseResource.h"

enum class ShapeType {
	Plane,
	Cube,
	Sphere,	

	Count
};

/// <summary>
/// CG2で作成した基本的(?)な描画情報
/// </summary>
class DrawResource : public BaseResource {
public:

	DrawResource() = default;
	~DrawResource();

	void Initialize(uint32_t vertexNum, uint32_t indexNum = 0);
	void Initialize(ShapeType type);

	/// <summary>
	/// 描画前準備(Render内で呼ばれるため、プログラム時に呼ぶ必要はない)
	/// </summary>
	void DrawReady() override;

	//親の行列をかける。毎フレーム呼ばないといけない。ごめん。
	void AddParentMatrix(const Matrix4x4& parentMatrix);

	ID3D12Resource* GetMaterialResource() const { return materialResource.Get(); }
	ID3D12Resource* GetParticleDataResource() const;
	ID3D12Resource* GetLightResource() const { return lightResource.Get(); }
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> GetTextureHandle() const { return textureHandle_; }

	std::vector<uint32_t> index_{};

	std::vector<Vector3> localPos_{};
	std::vector<Vector2> texcoord_{};
	std::vector<Vector3> normal_{};

	Vector3 position_{};
	Vector3 rotate_{};
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	Vector2 texturePos_{};
	Vector2 textureScale_{ 1.0f, 1.0f };
	float textureRotate_ = 0.0f;

	uint32_t color_ = 0xffffffff;

	bool enableLighting_ = false;
	Vector3 lightDirection_ = { 0.0f, 0.0f, 1.0f };
	uint32_t lightColor_ = 0xffffffff;
	float lightIntensity_ = 1.0f;

	Camera* camera_ = nullptr;

private:

	VertexData* vertex_ = nullptr;
	uint32_t* indices_ = nullptr;
	Material* material_ = nullptr;
	MatrixData* matrix_ = nullptr;
	DirectionalLightData* light_ = nullptr;

	uint32_t vertexNum_ = 0;
	uint32_t indexNum_ = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> matrixResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource = nullptr;

	std::vector<Matrix4x4> parentMatrices_{};

};
