#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Core/PSO/PSOConfig.h>
#include <Assets/AssetsLoader.h>
#include <Assets/OffScreen/OffScreenManager.h>
#include <Core/DXCommonFunction.h>
#include <spdlog/logger.h>

//todo SetPSOConfig内でのRootSignatureの作成命令の出力

enum class ShapeType {
	Plane,
	Cube,
	Sphere,

	Count
};

class BaseResource {
public:

	static void StaticInitialize(DXDevice* device, AssetsLoader* loader, OffScreenManager* offscreenManager, SRVManager* srvManager) {
		dxDevice_ = device;
		loader_ = loader;
		offscreenManager_ = offscreenManager;
		srvManager_ = srvManager;
	}

	BaseResource();
	BaseResource(const std::string debugName);

	virtual ~BaseResource();

	void Initialize();

	virtual void DrawReady() = 0;
	
	std::vector<D3D12_VERTEX_BUFFER_VIEW> GetVBV() const { return vbv_; }
	D3D12_INDEX_BUFFER_VIEW GetIBV() const { return ibv_; }

	std::vector<D3D12_GPU_VIRTUAL_ADDRESS> GetCBVAddress() const { return cbvAddresses_; }
	std::vector<std::shared_ptr<SRVHandle>> GetSRVHandle() { return srvHandles_; }

	uint32_t GetVertexNum() const { return vertexNum_; }
	uint32_t GetIndexNum() const { return indexNum_; }
	uint32_t GetInstanceNum() const { return instanceNum_; }

	bool IsUseTexture() const { return useTexture_; }

	PSOConfig psoConfig_{};
	
protected:

	template<typename T>
	void MakeVertex(T*& verPtr, uint32_t vertexNum);
	void MakeIndex(uint32_t*& indPtr, uint32_t indexNum);

	void SetModelData(AssetsID modelID);

	template<typename T>
	D3D12_GPU_VIRTUAL_ADDRESS MakeCBV(T*& ptr);

	template<typename T>
	std::shared_ptr<SRVHandle> MakeSRV(T*& ptr, uint32_t num);

	//高度な設定。GPUHandleだけ削除し、SRVは残す。
	void DeleteOnlyHandleSRV(int index);

	//今はまだテクスチャの配列増やすだけ
	void MakeUAV();

protected:

	static DXDevice* dxDevice_;
	static AssetsLoader* loader_;
	static OffScreenManager* offscreenManager_;
	static SRVManager* srvManager_;

	std::shared_ptr<spdlog::logger> logger_ = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbv_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

	uint32_t vertexNum_ = 0;
	uint32_t indexNum_ = 0;
	uint32_t instanceNum_ = 1;
	
private:

	struct Resource {
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
	};

	std::vector<Resource> allResources_{};

	std::vector<D3D12_GPU_VIRTUAL_ADDRESS> cbvAddresses_{};
	std::vector<std::shared_ptr<SRVHandle>> srvHandles_{};
	std::vector<std::shared_ptr<SRVHandle>> buffers_{};			//SRV保存用

	std::shared_ptr<spdlog::logger> debugLogger_ = nullptr;

	bool useTexture_ = false;
};



template<typename T>
inline void BaseResource::MakeVertex(T*& verPtr, uint32_t vertexNum) {
	vertexNum_ = vertexNum;

	//リソースの生成とマップ
	Resource res;
	res.res.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(T) * vertexNum_));
	res.res->Map(0, nullptr, (void**)&verPtr);
	allResources_.push_back(res);

	//vbvの作成
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = res.res->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(T) * vertexNum_;
	vbv.StrideInBytes = sizeof(T);
	vbv_.push_back(vbv);
}



template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS BaseResource::MakeCBV(T*& ptr) {

	Resource resource;
	resource.res.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(T)));
	resource.res->Map(0, nullptr, (void**)&ptr);
	cbvAddresses_.push_back(resource.res->GetGPUVirtualAddress());
	allResources_.push_back(resource);

	return resource.res->GetGPUVirtualAddress();
}

template<typename T>
inline std::shared_ptr<SRVHandle> BaseResource::MakeSRV(T*& ptr, uint32_t num) {

	Resource resource;
	resource.res.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(T) * num));
	resource.res->Map(0, nullptr, (void**)&ptr);
	allResources_.push_back(resource);

	auto srvHandle = std::make_shared<SRVHandle>();
	srvHandle->UpdateHandle(srvManager_);

	//ParticleDataのSRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = num;
	srvDesc.Buffer.StructureByteStride = sizeof(T);

	srvHandles_.push_back(srvHandle);

	dxDevice_->GetDevice()->CreateShaderResourceView(resource.res.Get(), &srvDesc, srvHandle->CPU);

	return srvHandle;
}
