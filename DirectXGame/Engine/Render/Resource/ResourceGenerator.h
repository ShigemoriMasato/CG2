#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <Core/SRVManager.h>
#include <Core/PSO/Shelf/RootSignatureShelf.h>
#include <Core/PSO/PSOConfig.h>
#include <Assets/AssetsLoader.h>
#include <Core/DXCommonFunction.h>

class Render;

enum class Primitive {
	Triangle,
	Plane,
	Cube,
	Sphere,
	MaxCount
};

struct Resource {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
};

class ResourceGenerator {
public:

	ResourceGenerator();
	~ResourceGenerator();

	static void StaticInitialize(ID3D12Device* device, SRVManager* srv, AssetsLoader* assets);
	static void SetRootSignatureShelf(RootSignatureShelf* rsShelf) { rsShelf_ = rsShelf; }

	void Initialize();

	void SetVertex(Primitive type);
	void SetVertex(AssetsID modelID);
	
	template<typename T>
	void AddCBV(T*& ptr, ShaderType type);

	template<typename T>
	void AddSRV(T*& ptr, uint32_t size, ShaderType type);

	void AddUAV(ShaderType type);

	//ResourceGeneratorの設定がすべて終わった時に呼ぶ
	void Create();

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indices_;

	std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, ShaderType>> GetCBVAddresses() const { return cbvAddresses_; }
	std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> GetSRVAddresses() const { return srvAddresses_; }
	std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>>GetUAVAddresses() const { return uavAddresses_; }
	D3D12_VERTEX_BUFFER_VIEW GetVBV() const { return vbv_; }
	D3D12_INDEX_BUFFER_VIEW GetIBV() const { return ibv_; }

	//Create後も変更OK(RSをのぞく)
	PSOConfig psoConfig_;

private:

	static ID3D12Device* device_;
	static SRVManager* srvManager_;
	static AssetsLoader* assetsLoader_;
	static RootSignatureShelf* rsShelf_;

	static D3D12_GPU_DESCRIPTOR_HANDLE initializeTextureHandle_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbv_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibv_ = {};

	//CommandListへの提出用
	std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, ShaderType>> cbvAddresses_;
	std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> srvAddresses_;
	std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> uavAddresses_;

	//保存用
	std::vector<Resource> resources_;
	std::vector<SRVHandle> srvHandles_;
};

template<typename T>
inline void ResourceGenerator::AddCBV(T*& ptr, ShaderType type) {
	Resource res;
	res.resource.Attach(CreateBufferResource(device_, sizeof(T)));
	res.resource->Map(0, nullptr, (void**)&ptr);
	resources_.emplace_back(res);

	cbvAddresses_.push_back({ res.resource->GetGPUVirtualAddress(), type });
}

template<typename T>
inline void ResourceGenerator::AddSRV(T*& ptr, uint32_t num, ShaderType type) {
	Resource res;
	UINT64 size = sizeof(T) * num;
	res.resource.Attach(CreateBufferResource(device_, size));
	res.resource->Map(0, nullptr, (void**)&ptr);
	resources_.emplace_back(res);

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = num;
	srvDesc.Buffer.StructureByteStride = sizeof(T);

	SRVHandle srvHandle;
	srvHandle.UpdateHandle(srvManager_);

	device_->CreateShaderResourceView(res.resource.Get(), &srvDesc, srvHandle.CPU);

	srvHandles_.push_back(srvHandle);
	srvAddresses_.push_back({ srvHandle.GPU, type });
}
