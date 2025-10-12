#include "Render.h"
#include <Core/DXCommonFunction.h>
#include <imgui/imgui_impl_dx12.h>

Render::Render(DXDevice* device) {
    device_ = device;
    logger_ = std::make_unique<Logger>();
	logger_->RegistLogFile("Render");
    psoEditor_ = std::make_unique<PSOEditor>(device_->GetDevice());

	// ============================================

    //CommandQueue
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    //コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandQueue\n");

    //CommandAllocator
    hr = device_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    //コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandAllocator\n");

    //CommandList
    hr = device_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    //コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create CommandList\n");

}

Render::~Render() {
}

void Render::Initialize(TextureManager* textureManager, OffScreenManager* offScreenManager, SRVManager* srvManager) {
    
	auto windowSize = device_->GetWindowSize();

    //スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = windowSize.first;	                        //ウィンドウ幅
    swapChainDesc.Height = windowSize.second;	                    //ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      	    //色の形式
    swapChainDesc.SampleDesc.Count = 1;	                            //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2;	//ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   	//モニタに映したら捨てる

    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT hr = device_->GetDxgiFactory()->CreateSwapChainForHwnd(
        commandQueue.Get(),		        		//コマンドキュー
        device_->GetHwnd(),			            //ウィンドウハンドル
        &swapChainDesc,	        		        //設定
        nullptr,		    	    		    //モニタの設定
        nullptr,			    		        //出力の設定
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));	//スワップチェーンのポインタ
    assert(SUCCEEDED(hr));
    logger_->Log("Complete create SwapChain\n");

    //SwapChainからResourceを取得する
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    //ディスクリプタヒープの生成(swapchain用なので二つ)
	ID3D12DescriptorHeap* rawHeap = nullptr;
    rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	rtvDescriptorHeap.Attach(rawHeap);
    logger_->Log("Complete create DescriptorHeap\n");

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //まず一つ目をつくる。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある。
    rtvHandles[0] = rtvStartHandle;
    device_->GetDevice()->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    //二つ目のディスクリプタハンドルを得る
    rtvHandles[1].ptr = rtvHandles[0].ptr + device_->GetDescriptorSizeRTV();
    //二つ目を作る
    device_->GetDevice()->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
    logger_->Log("Complete create RenderTargetView\n");

#pragma endregion

#pragma region Fence

    //初期値0でフェンスを作る
    hr = device_->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

#pragma endregion

    //=======================================================

    rawHeap = CreateDescriptorHeap(device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	dsvDescriptorHeap.Attach(rawHeap);
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	ID3D12Resource* rawResource = nullptr;
    rawResource = CreateDepthStencilTextureResource(device_->GetDevice(), windowSize.first, windowSize.second);
	depthStencilResource.Attach(rawResource);

    device_->GetDevice()->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    //PSOの初期化
    psoEditor_->Initialize(device_->GetDevice());

	textureManager_ = textureManager;
	offScreenManager_ = offScreenManager;
	srvManager_ = srvManager;

	//初期化の段階で一度コマンドリストの中身をすべて実行する
    hr = commandList->Close();
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    //Fenceの値を更新
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        //イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    //次のフレームのためのcommandReset
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);
}

void Render::PreDraw(int offscreenHandle) {
    //PSOとRootSignatureを初期化する
    if (isFrameFirst_) {
        psoEditor_->FrameInitialize(commandList.Get());
        ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetHeap() };
		commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		isFrameFirst_ = false;
    } else {
        
    }

    offScreenHandle_ = offscreenHandle;

    //描画する画面が指定されている場合はそちらにする
    if (offscreenHandle != -1) {
		auto offscreen = offScreenManager_->GetOffScreenData(offscreenHandle);
		PreDrawOffScreen(offscreen);
        return;
    }

	//そうでない場合はスワップチェーンのバッファに描画する
    PreDrawSwapChain();

}

void Render::Draw(DrawResource* resource) {

	resource->DrawReady();

    auto vertexBufferView = resource->GetVertexBufferView();
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	uint32_t indexNum = resource->GetIndexNum();
    if (indexNum != 0) {
        auto indexBufferView = resource->GetIndexBufferView();
		commandList->IASetIndexBuffer(&indexBufferView);
    }

    psoEditor_->SetPSOConfig(resource->psoConfig_);
    psoEditor_->Setting(commandList.Get());

    //マテリアルのポインタを設定
    commandList->SetGraphicsRootConstantBufferView(0, resource->GetMaterialResource()->GetGPUVirtualAddress());

    //Matrixのポインタを設定
    if (resource->GetParticleDataResource()) {
        commandList->SetGraphicsRootConstantBufferView(1, resource->GetParticleDataResource()->GetGPUVirtualAddress());
        //Texture
        commandList->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureData(resource->textureHandle_)->GetTextureGPUHandle());
        //Lightのポインタを設定
        commandList->SetGraphicsRootConstantBufferView(3, resource->GetLightResource()->GetGPUVirtualAddress());
    } else {
        //Texture
        commandList->SetGraphicsRootDescriptorTable(1, textureManager_->GetTextureData(resource->textureHandle_)->GetTextureGPUHandle());
        //Lightのポインタを設定
        commandList->SetGraphicsRootConstantBufferView(2, resource->GetLightResource()->GetGPUVirtualAddress());
    }

    if (indexNum != 0) {
        //インデックスがある場合は、インデックスを設定して描画
        commandList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
    } else {
        //インデックスがない場合は、インデックスなしで描画
        commandList->DrawInstanced(resource->GetVertexNum(), 1, 0, 0);
    }

}

void Render::Draw(ModelResource* resource) {
    resource->DrawReady();

	psoEditor_->SetPSOConfig(resource->psoConfig_);
	psoEditor_->Setting(commandList.Get());
    auto drawData = resource->GetModelDrawDatas();

    for(const auto& [name, drawData] : drawData) {
        auto vertexBufferView = drawData.vertexBufferView;
        commandList->IASetVertexBuffers(0, 1, drawData.vertexBufferView);
        auto indexBufferView = drawData.indexBufferView;
        commandList->IASetIndexBuffer(drawData.indexBufferView);
        //Material
		commandList->SetGraphicsRootConstantBufferView(0, resource->GetMaterialResource()->GetGPUVirtualAddress());
        //Matrixのポインタを設定
        commandList->SetGraphicsRootDescriptorTable(1, resource->GetMatrixSRVDesc());
        //Texture
        commandList->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureData(drawData.textureHandle)->GetTextureGPUHandle());
        //インデックスがある場合は、インデックスを設定して描画
        commandList->DrawIndexedInstanced(drawData.indexNum, 1, 0, 0, 0);
	}

}

void Render::Draw(ParticleResource* resource) {

    resource->DrawReady();

    auto vertexBufferView = resource->GetVertexBufferView();
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    uint32_t indexNum = resource->GetIndexNum();
	//Indexがある場合はViewを取り込む
    if (indexNum != 0) {
        auto indexBufferView = resource->GetIndexBufferView();
        commandList->IASetIndexBuffer(&indexBufferView);
    }

    psoEditor_->SetPSOConfig(resource->psoConfig_);
    psoEditor_->Setting(commandList.Get());

	//ParticleDataのポインタを設定
	commandList->SetGraphicsRootDescriptorTable(0, resource->GetParticleDataSRVDesc());
	//Texture
	commandList->SetGraphicsRootDescriptorTable(1, textureManager_->GetTextureData(resource->textureHandle_)->GetTextureGPUHandle());

    if (indexNum != 0) {
        //インデックスがある場合は、インデックスを設定して描画
        commandList->DrawIndexedInstanced(indexNum, resource->GetInstanceNum(), 0, 0, 0);
    } else {
        //インデックスがない場合は、インデックスなしで描画
        commandList->DrawInstanced(resource->GetVertexNum(), resource->GetInstanceNum(), 0, 0);
    }

}

void Render::PostDraw(ImGuiWrapper* imguiRap) {
    if (offScreenHandle_ != -1) {
        ResetResourceBarrier();
        PreDraw();
    }

    if (isFrameFirst_) {
        PreDraw();
    }

    ImGuiOperator::EndFrame(commandList.Get());

    ResetResourceBarrier();

	//コマンドリストのクローズ
    HRESULT hr = commandList->Close();
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);
    //GPUとOSに画面の交換を行うよう通知する
    hr = swapChain->Present(1, 0);
    
    if (true) {
        // ログに hr を出す (8桁16進などで)
		logger_->Log(std::format("Failed to Present. hr = 0x{}\n", hr));
    }

    //presentするとBarrierが自動でCommonになる。
    int backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    resourcestates_[backBufferIndex] = D3D12_RESOURCE_STATE_COMMON;

    //Fenceの値を更新
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        //イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    //次のフレームのためのcommandReset
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), nullptr);

    isFrameFirst_ = true;
}

ImGui_ImplDX12_InitInfo Render::GetImGuiInitInfo(SRVManager* srv) {
    ImGui_ImplDX12_InitInfo info;
	info.Device = device_->GetDevice();
	info.NumFramesInFlight = 2;
	info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	info.CommandQueue = commandQueue.Get();
	info.SrvDescriptorHeap = srv->GetHeap();
	info.LegacySingleSrvCpuDescriptor = srv->GetCPUHandle();
    info.LegacySingleSrvGpuDescriptor = srv->GetGPUHandle();
	return info;
}

void Render::PreDrawSwapChain() {
    //描画する画面を取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    //バリアを変更
    InsertBarrier(commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, resourcestates_[backBufferIndex], swapChainResources[backBufferIndex].Get());

    //RenderTargetの切り替え
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

    //指定した色で画面全体をクリアする
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor_, 0, nullptr);

    //深度バッファをクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //ビューポート
    D3D12_VIEWPORT viewport{};
    //クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(device_->GetWindowSize().first);
    viewport.Height = static_cast<float>(device_->GetWindowSize().second);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //シザー矩形
    D3D12_RECT scissorRect{};
    //基本的にビューポートと同じく刑が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = device_->GetWindowSize().first;
    scissorRect.top = 0;
    scissorRect.bottom = device_->GetWindowSize().second;

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
}

void Render::PreDrawOffScreen(OffScreenData* offScreen) {
	offScreenManager_->GetOffScreenData(offScreenHandle_)->DrawReady(commandList.Get());
}

void Render::ResetResourceBarrier() {
    if (offScreenHandle_ != -1) {
        offScreenManager_->GetOffScreenData(offScreenHandle_)->EditBarrier(commandList.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    } else {
		int backBufferIndex = swapChain->GetCurrentBackBufferIndex();
        InsertBarrier(commandList.Get(), D3D12_RESOURCE_STATE_PRESENT, resourcestates_[backBufferIndex], swapChainResources[backBufferIndex].Get());
    }
}
