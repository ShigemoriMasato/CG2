#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include <strsafe.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dbghelp.h>
#include <dxgidebug.h>
#include <memory>
#include <dxcapi.h>
#include <unordered_map>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "Logger.h"
#include "Vector4.h"

class MyDirectX {
public:
	MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight);
	~MyDirectX();

	void Initialize();

	void ClearScreen();

	void EndFrame();

	void Finalize();

	void CreateWindowForApp();

	void InitDirectX();

private:

	void InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
		D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	Logger* logger;

	const int32_t kClientWidth;		//ウィンドウ幅
	const int32_t kClientHeight;	//ウィンドウ高さ
	float* clearColor;		//windowの色

	HWND hwnd;

	//DirectXCommon
	ID3D12Debug1* debugController = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	ID3D12Device* device = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	uint64_t fenceValue;


	//スワップチェーンの設定
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr, nullptr };
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceEvent;

	ID3D12Resource* backBuffers[2] = { nullptr, nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates;
};

