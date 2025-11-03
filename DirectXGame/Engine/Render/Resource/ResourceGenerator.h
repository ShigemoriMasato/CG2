#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <Core/SRVManager.h>
#include <Assets/AssetsLoader.h>

class Render;

enum class ViewType {
	CBV,
	SRV,
	UAV
};

enum class Primitive {
	Triangle,
	Plane,
	Cube,
	Sphere
};

struct Resource {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
};

class ResourceGenerator {
public:

	ResourceGenerator();
	~ResourceGenerator();

	static void StaticInitialize(ID3D12Device* device, SRVManager* srv, AssetsLoader* assets);

	void Initialize();

	void SetVertex(Primitive type);
	void SetVertex(AssetsID modelID);
	
	template<typename T>
	void CreateResource(ViewType view, T*& res);

	//ResourceGeneratorの設定がすべて終わった時に呼ぶ
	void Create();

private:

	static ID3D12Device* device_;
	static SRVManager* srvManager_;
	static AssetsLoader* assetsLoader_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibv_ = {};

	std::vector<Resource> resources_;
	std::vector<std::pair<ViewType, UINT64>> viewTypes_;
};

template<typename T>
void ResourceGenerator::CreateResource(ViewType view, T*& res) {

}
