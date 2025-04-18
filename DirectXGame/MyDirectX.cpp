#include "MyDirectX.h"

namespace {
    //ウィンドウプロシージャ
    LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        //メッセージに応じてゲーム固有の処理を行う
        switch (msg) {
        case WM_DESTROY:
            //アプリを落とす
            PostQuitMessage(0);
            return 0;
        }

        //標準のメッセージ
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
        //取得した時間を名前にしたファイルを作成し、そのなかにDumpを出力する
        SYSTEMTIME time;
        GetLocalTime(&time);
        wchar_t filePath[MAX_PATH] = { 0 };
        CreateDirectory(L"./Dumps", nullptr);
        StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
        HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
        //processID(このexeのID)とクラッシュ(例外)の発生したthreadIDを取得
        DWORD processId = GetCurrentProcessId();
        DWORD threadId = GetCurrentThreadId();
        //設定情報を入力
        MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
        minidumpInformation.ThreadId = threadId;
        minidumpInformation.ExceptionPointers = exception;
        minidumpInformation.ClientPointers = TRUE;
        //Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
        MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
        //他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
        return EXCEPTION_EXECUTE_HANDLER;
    }

    IDxcBlob* CompileShader(
        //CompilerするShaderファイルへのパス
        const std::wstring& filePath,
        //Compilerに使用するprofile
        const wchar_t* profile,
        //初期化で生成したものを3つ
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler,
        std::shared_ptr<Logger> logger) {

        logger->Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));

        //hlslファイルを読む
        IDxcBlobEncoding* shaderSource = nullptr;
        HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
        //読めなかったら止める
        assert(SUCCEEDED(hr));
        //読み込んだファイルの内容を設定する
        DxcBuffer shaderSourceBuffer;
        shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
        shaderSourceBuffer.Size = shaderSource->GetBufferSize();
        shaderSourceBuffer.Encoding = DXC_CP_UTF8;//utf8の文字コードであることを通知

        LPCWSTR arguments[] = {
            filePath.c_str(),	//コンパイル対象のhlslファイル名
            L"-E", L"main",     //エントリーポイントの指定。基本的にmain以外には市内
            L"-T", profile,    //ShaderProfileの設定
            L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
            L"-Od",     //最適化を行わない
            L"-Zpr",     //メモリレイアウトは行優先
        };
        //実際にShaderをコンパイルする
        IDxcResult* shaderResult = nullptr;
        hr = dxcCompiler->Compile(
            &shaderSourceBuffer,	//読み込んだファイル
            arguments,			    //コンパイルオプション
            _countof(arguments),	//コンパイルオプションの数
            includeHandler,		    //includeが含まれた諸々
            IID_PPV_ARGS(&shaderResult)		//コンパイル結果
        );
        //コンパイルエラーではなくdxcが起動できないなどの致命的な状況
        assert(SUCCEEDED(hr));

        //警告・エラーが出てたらログに出して止める
        IDxcBlobUtf8* shaderError = nullptr;
        shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
        if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
            logger->Log(shaderError->GetStringPointer());
            //警告・エラーが起きている状態なので止める
            assert(false);
        }

        //コンパイル結果から実行用のバイナリ部分を取得
        IDxcBlob* shaderBlob = nullptr;
        hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
        assert(SUCCEEDED(hr));
        //成功したログを出す
        logger->Log(ConvertString(std::format(L"Compile Successd, path: {}, profile{}\n", filePath, profile)));
        //もう使わないリソースを開放
        shaderSource->Release();
        shaderResult->Release();
        //実行用のバイナリを返却
        return shaderBlob;
    }

    ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
        //頂点リソース用のヒープの設定
        D3D12_HEAP_PROPERTIES uploadHeapProperties{};
        uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//uploadHeapを使う
        //頂点リソースの設定
        D3D12_RESOURCE_DESC bufferResourceDesc{};
        //バッファリソース、テクスチャの場合はまた別の設定をする
        bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferResourceDesc.Width = sizeof(Vector4) * 3;//リソースのサイズ。今回はVector4を3頂点文
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Height = 1;
        bufferResourceDesc.DepthOrArraySize = 1;
        bufferResourceDesc.MipLevels = 1;
        bufferResourceDesc.SampleDesc.Count = 1;
        //バッファの場合はこれにする決まり
        bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ID3D12Resource* bufferResource = nullptr;

        HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
            &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&bufferResource));
        assert(SUCCEEDED(hr));

        return bufferResource;
    }
}

MyDirectX::MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight) : 
kClientWidth(kWindowWidth), 
kClientHeight(kWindowHeight),
clearColor(new float[4] {0.1f, 0.25f, 0.5f, 1.0f}),
logger(new Logger("DirectX12")),
fenceValue(0) {
    resourceStates[swapChainResources[0]] = D3D12_RESOURCE_STATE_PRESENT;
    resourceStates[swapChainResources[1]] = D3D12_RESOURCE_STATE_PRESENT;
}

MyDirectX::~MyDirectX() {
    delete logger;
	delete[] clearColor;
}

void MyDirectX::Initialize() {
    CreateWindowForApp();
	InitDirectX();
}

void MyDirectX::CreateWindowForApp() {
    WNDCLASS wc{};
    //ウィンドウプロシージャ
    wc.lpfnWndProc = windowProc;
    //ウィンドウクラス名
    wc.lpszClassName = L"CG2WindowClass";
    //インスタンスハンドル
    wc.hInstance = GetModuleHandleA(nullptr);
    //カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスの登録
    RegisterClass(&wc);

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, kClientWidth, kClientHeight };

    //クライアント領域をもとに実際のサイズにwrcに変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    //ウィンドウの作成
    hwnd = CreateWindow(
        wc.lpszClassName,			//利用するクラスの名前
        L"CG2",						//タイトルバーの文字
        WS_OVERLAPPEDWINDOW,		//よく見るウィンドウスタイル
        CW_USEDEFAULT,				//表示x座標
        CW_USEDEFAULT,				//表示y座標
        wrc.right - wrc.left,		//ウィンドウ幅
        wrc.bottom - wrc.top,		//ウィンドウ高さ
        nullptr,					//親ウィンドウハンドル
        nullptr,					//メニューハンドル
        wc.hInstance,				//インスタンスハンドル
        nullptr);					//オプション

    ShowWindow(hwnd, SW_SHOW);	//ウィンドウを表示する
}

void MyDirectX::InitDirectX() {
#ifdef _DEBUG

    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        //デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        //さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    //HRESULTはWindows系のエラーコードであり、
    //関数が成功したかどうかをSUCCEEDEDマクロで判定する
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    //初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    //どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr));
    //一番いいアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        //アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);
        //取得できなければエラー
        assert(SUCCEEDED(hr));
        //ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            //採用したアダプタの情報をログに出力
            logger->Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

            break;
        }
        //ソフトウェアアダプタの場合はなかったことに
        useAdapter = nullptr;
    }

    //適切なアダプタが見つからなかった場合は起動できない
    assert(useAdapter != nullptr);

    //機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    std::string featureLevelStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        //採用したアダプタでデバイスを生成
        hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
        //指定した機能レベルでデバイスが生成できたか確認
        if (SUCCEEDED(hr)) {
            //生成できたのでログを出力してループを抜ける
            logger->Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
            break;
        }
    }

    //デバイスの生成が上手くいかなかったので起動できない
    assert(device != nullptr);
    logger->Log("Complete create D3D12Device\n");

#ifdef _DEBUG

    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        //やばいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        //警告の時に止まる(これをコメントアウトすると解放していないObjectを特定できる(?))
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        //抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            //windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            //https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        //抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        //指定したメッセージの行事を抑制する
        infoQueue->PushStorageFilter(&filter);

        //解放
        infoQueue->Release();
    }

#endif

    //=======================================================
    //          画面を塗りつぶす素材用意
    //=======================================================

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    //コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    logger->Log("Complete create CommandQueue\n");

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    //コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    logger->Log("Complete create CommandAllocator\n");

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
    //コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    logger->Log("Complete create CommandList\n");

    //スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = kClientWidth;	                            //ウィンドウ幅
    swapChainDesc.Height = kClientHeight;	                        //ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      	    //色の形式
    swapChainDesc.SampleDesc.Count = 1;	                            //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2;	//ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   	//モニタに映したら捨てる

    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue,		        		//コマンドキュー
        hwnd,			           			//ウィンドウハンドル
        &swapChainDesc,	        		    //設定
        nullptr,		    	    		//モニタの設定
        nullptr,			    		    //出力の設定
        (IDXGISwapChain1**)&swapChain);	//スワップチェーンのポインタ

    assert(SUCCEEDED(hr));
    logger->Log("Complete create SwapChain\n");


    //ディスクリプタヒープの生成
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダーターゲットビュー用
    rtvDescriptorHeapDesc.NumDescriptors = 2;	//ダブルバッファ用に2つ。多くてもよい
    rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//特にフラグはなし
    hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));

    //ディスクリプタヒープの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    logger->Log("Complete create DescriptorHeap\n");

    //SwapChainからResourceを取得する
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    //スワップチェーンのリソース取得がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //まず一つ目をつくる。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある。
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
    //二つ目のディスクリプタハンドルを得る(自力で)
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //二つ目を作る
    device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

    logger->Log("Complete create RenderTargetView\n");

    //初期値0でフェンスを作る
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

}


void MyDirectX::ClearScreen() {
    //これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	logger->Log(ConvertString(std::format(L"BackBufferIndex: {}\n", backBufferIndex)));

	InsertBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, swapChainResources[backBufferIndex]);

    //描画先のRTVを設定する
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
    //指定した色で画面全体をクリアする
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

}

void MyDirectX::EndFrame() {
	InsertBarrier(commandList, D3D12_RESOURCE_STATE_PRESENT, swapChainResources[swapChain->GetCurrentBackBufferIndex()]);

    HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr));


    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList };
    commandQueue->ExecuteCommandLists(1, commandLists);
    //GPUとOSに画面の交換を行うよう通知する
    swapChain->Present(1, 0);

    //Fenceの値を更新
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence, fenceValue);

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        //イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    commandAllocator->Reset();
    commandList->Reset(commandAllocator, nullptr);

}

void MyDirectX::Finalize() {
    CloseHandle(fenceEvent);
    fence->Release();
    rtvDescriptorHeap->Release();
    swapChainResources[0]->Release();
    swapChainResources[1]->Release();
    swapChain->Release();
    commandList->Release();
    commandAllocator->Release();
    commandQueue->Release();
    device->Release();
    useAdapter->Release();
    dxgiFactory->Release();
#ifdef _DEBUG
    debugController->Release();
#endif
    CloseWindow(hwnd);

    IDXGIDebug* debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }

}

void MyDirectX::InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
    D3D12_RESOURCE_BARRIER_TYPE type, D3D12_RESOURCE_BARRIER_FLAGS flags) {
    // 初期化されていなければ Present で初期化
    if (resourceStates.find(pResource) == resourceStates.end()) {
        resourceStates[pResource] = D3D12_RESOURCE_STATE_PRESENT;
    }

    D3D12_RESOURCE_STATES stateBefore = resourceStates[pResource];

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = type;
    barrier.Flags = flags;
    barrier.Transition.pResource = pResource;
    barrier.Transition.StateBefore = stateBefore;
    barrier.Transition.StateAfter = stateAfter;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    int beforeInt = static_cast<int>(stateBefore);
    int afterInt = static_cast<int>(stateAfter);
    std::wstring formatted = std::format(L"InsertBarrier, Before: {}, After: {}\n", beforeInt, afterInt);
    logger->Log(ConvertString(formatted));

    commandList->ResourceBarrier(1, &barrier);

    // 状態を更新
    resourceStates[pResource] = stateAfter;
}
