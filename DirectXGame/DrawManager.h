#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <dxcapi.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "Logger.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MyMath.h"

class DrawManager {
public:
	DrawManager(int32_t kWindowWidth, int32_t kWindowHeight);

	void Initialize(ID3D12Device** device, ID3D12GraphicsCommandList** commandList);

	void Finalize();

	int ReadTexture(std::string path);

	void Begin(IDXGISwapChain4* swapChain, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles);

	void DrawTriangle2D(Vector4 left, Vector4 top, Vector4 right, Matrix4x4 wvpMatrix, Vector4 color, int32_t textureHandle);

	void DrawTriangle3D();

	void DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 wvpMatrix, Vector4 color, int32_t textureHandle);

	void DrawSphere(Matrix4x4 wvpMatrix, Vector4 color, int32_t textureHandle);

	void End(IDXGISwapChain4* swapChain);

private:

	void ClearScreen(IDXGISwapChain4* swapChain, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles);

	//借り物
	ID3D12Device** device_ = nullptr;
	ID3D12GraphicsCommandList** commandList_ = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;

	//必需品
	const int32_t kWindowWidth_;
	const int32_t kWindowHeight_;
	ID3D12Resource* depthStencilResource = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	ID3D10Blob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	IDxcBlob* pixelShaderBlob = nullptr;
	IDxcBlob* vertexShaderBlob = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	std::shared_ptr<Logger> logger;
	uint32_t descriptorSizeSRV_ = 0;
	uint32_t descriptorSizeRTV_ = 0;
	uint32_t descriptorSizeDSV_ = 0;

	//画像用
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandleGPU;
	uint32_t readTextureCount_;
	std::vector<ID3D12Resource*> textureResource;
	std::vector<ID3D12Resource*> intermediateResource;

	//実際に描画するときのデータ集
	std::vector<std::vector<ID3D12Resource*>> vertexResource;
	std::vector<std::vector<ID3D12Resource*>> wvpResource;
	std::vector<std::vector<ID3D12Resource*>> materialResource;
};

