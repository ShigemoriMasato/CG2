#include "OffScreenData.h"
#include <Core/DXCommonFunction.h>

int OffScreenData::offScreenTextureCount_ = 0;

OffScreenData::OffScreenData(int width, int height, float* clearColor, DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager, ID3D12DescriptorHeap* rtv) {

    //PostEffect用のリソースの作成
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    for(int i = 0; i < 4; ++i) {
        clearValue.Color[i] = clearColor[i];
        clearColor_[i] = clearColor[i];
	}

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;              // defaultのヒープを使用

    HRESULT hr = device->GetDevice()->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&textureResource_)
    );
    assert(SUCCEEDED(hr) && "Failed to create off-screen resource");

    rtvHandle_.ptr = rtv->GetCPUDescriptorHandleForHeapStart().ptr + device->GetDescriptorSizeRTV() * (offScreenTextureCount_++);

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDescOffScreen{};
    rtvDescOffScreen.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//出力結果をSRGBに変換して書き込む
    rtvDescOffScreen.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    device->GetDevice()->CreateRenderTargetView(textureResource_.Get(), &rtvDescOffScreen, rtvHandle_);

    // metadataがないのでフォーマットとミップ数は手動設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    //!mipmapを使うかどうかは今後要検討
    srvDesc.Texture2D.MipLevels = 1;

    // SRV用ディスクリプタ位置を確保
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvManager->GetCPUHandle();
	gpuHandle_ = srvManager->GetGPUHandle();

    // SRVを作成
    device->GetDevice()->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU);

    //バリアの初期状態を設定
    resourceState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    dsvDescriptorHeap_.Attach(CreateDescriptorHeap(device->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false));
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    depthStencilResource_.Attach(CreateDepthStencilTextureResource(device->GetDevice(), width, height));

    device->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	width_ = width;
	height_ = height;
}

void OffScreenData::EditBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, D3D12_RESOURCE_BARRIER_TYPE type, D3D12_RESOURCE_BARRIER_FLAGS flags) {
	InsertBarrier(commandlist, stateAfter, resourceState_, textureResource_.Get(), type, flags);
}

void OffScreenData::DrawReady(ID3D12GraphicsCommandList* commandList) {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    //バリアを変更
    InsertBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, resourceState_, textureResource_.Get());

    //RenderTargetの切り替え
    commandList->OMSetRenderTargets(1, &rtvHandle_, false, &dsvHandle);

    //指定した色で画面全体をクリアする
    commandList->ClearRenderTargetView(rtvHandle_, clearColor_, 0, nullptr);

    //深度バッファをクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //ビューポート
    D3D12_VIEWPORT viewport{};
    //クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(width_);
    viewport.Height = static_cast<float>(height_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //シザー矩形
    D3D12_RECT scissorRect{};
    //基本的にビューポートと同じく刑が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = width_;
    scissorRect.top = 0;
    scissorRect.bottom = height_;

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

}
