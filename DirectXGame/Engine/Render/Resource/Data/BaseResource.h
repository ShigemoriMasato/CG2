#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <Core/DXDevice.h>
#include <Core/PSO/PSOConfig.h>
#include <Assets/AssetsLoader.h>
#include <Core/DXCommonFunction.h>
#include <spdlog/logger.h>

class BaseResource {
public:
	BaseResource() = default;
	BaseResource(const std::string debugName);

	virtual ~BaseResource() = default;

	virtual void DrawReady() = 0;

	//使用しないかもだけどいちいち面倒なのでここで宣言する。気になる場合はprivateでオーバーライドすること
	void SetTextureHandle(AssetsID id, uint32_t index);
	void SetTextureHandle(ScreenID offScreenIndex, uint32_t textureIndex);

protected:

	template<typename T>
	void MakeVertex(T*& verPtr, uint32_t vertexNum);
	void MakeIndex(uint32_t*& indPtr, uint32_t indexNum);

	static DXDevice* dxDevice_;
	static AssetsLoader* loader_;

	std::shared_ptr<spdlog::logger> logger_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbv_;
	std::vector<D3D12_INDEX_BUFFER_VIEW> ibv_;

	PSOConfig psoConfig_{};
	
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureHandle_{};
};

template<typename T>
inline void BaseResource::MakeVertex(T*& verPtr, uint32_t vertexNum) {
	//リソースの生成とマップ
	vertexResource.Attach(CreateBufferResource(dxDevice_->GetDevice(), sizeof(VertexData) * vertexNum));
	vertexResource->Map(0, nullptr, (void**)verPtr);

	//vbvの作成
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexData) * vertexNum;
	vbv.StrideInBytes = sizeof(VertexData);
	vbv_.push_back(vbv);
}
