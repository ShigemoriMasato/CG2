#pragma once
#include <Core/DXDevice.h>
#include <Core/PSO/PSOEditor.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/OffScreen/OffScreenManager.h>
#include <Render/ImGuiWrapper.h>
#include <imgui/imgui_impl_dx12.h>

class Render {
public:

	Render(DXDevice* device);
	~Render();

	void Initialize(SRVManager* srvManager);

	void PreDraw(OffScreenIndex index = OffScreenIndex::SwapChain, bool isClear = true);
	//void Draw(DrawResource* resource);
	//void Draw(PostEffectResource* resource);
	void PostDraw();

	void EndFrame(bool swapchainPresent);

	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	ImGui_ImplDX12_InitInfo GetImGuiInitInfo(SRVManager* srv);

	void SetClearColor(float r, float g, float b, float a) {
		clearColor_[0] = r;
		clearColor_[1] = g;
		clearColor_[2] = b;
		clearColor_[3] = a;
	}

private:

	void PreDrawSwapChain(bool isClear);
	void PreDrawOffScreen(bool isClear);

	void ResetResourceBarrier();

	//Device
	DXDevice* device_ = nullptr;

	//Command関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	//RTV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//スワップチェーンの設定
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr, nullptr };
	D3D12_RESOURCE_STATES resourcestates_[2] = { D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_PRESENT };
	float clearColor_[4] = { 0.0f,0.0f,0.0f,1.0f };
	OffScreenIndex offScreenHandle_ = OffScreenIndex::SwapChain;

	bool isFrameFirst_ = true;	//PreDrawが初回かどうか

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	uint64_t fenceValue;

	//PSO管理
	std::unique_ptr<PSOEditor> psoEditor_ = nullptr;

	//Depth
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;

	//テクスチャ
	TextureManager* textureManager_ = nullptr;
	OffScreenManager* offScreenManager_ = nullptr;

	//SRV
	SRVManager* srvManager_ = nullptr;
};
