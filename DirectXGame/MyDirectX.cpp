#include "MyDirectX.h"

namespace {
    //ウィンドウプロシージャ
    LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        //imguiのウィンドウプロシージャを呼ぶ
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
            return true;
        }

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
        Logger* logger) {

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
        bufferResourceDesc.Width = sizeInBytes;
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

    ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible) {
        //ディスクリプタヒープの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
        descriptorHeapDesc.Type = type;	//ディスクリプタの種類
        descriptorHeapDesc.NumDescriptors = numDescriptors;	//ディスクリプタの数
        descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	//シェーダーからアクセスできるようにする
        ID3D12DescriptorHeap* descriptorHeap = nullptr;
        HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
        assert(SUCCEEDED(hr));
        return descriptorHeap;
    }

    DirectX::ScratchImage LoadTexture(const std::string& filePath) {
        //テクスチャファイルを読んでプログラムで扱えるようにする
        DirectX::ScratchImage image{};
        std::wstring filePathW = ConvertString(filePath);
        HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
        assert(SUCCEEDED(hr));

        //ミニマップの作製
        DirectX::ScratchImage mipImages{};
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
        assert(SUCCEEDED(hr));

        return mipImages;
    }

    ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
        //metadataを基にResourceの設定
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = UINT(metadata.width);
        resourceDesc.Height = UINT(metadata.height);
        resourceDesc.MipLevels = UINT16(metadata.mipLevels);
        resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
        resourceDesc.Format = metadata.format;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

        //利用するHeapの設定。非常に特殊な運用。02-04exで一般的なケース版がある。
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        //Resourceの生成
        ID3D12Resource* resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE,
            &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
            IID_PPV_ARGS(&resource));
        assert(SUCCEEDED(hr));
        return resource;
    }

    ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
        //生成するResourceの設定
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.MipLevels = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        //利用するヒープの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        //深度値のクリア設定
        D3D12_CLEAR_VALUE depthClearValue{};
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        //Resourceの生成
        ID3D12Resource* resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue,
            IID_PPV_ARGS(&resource));
        assert(SUCCEEDED(hr));

        return resource;
    }

    [[nodiscard]]
    ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
        uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
        ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
        UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = texture;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier);
        return intermediateResource;
    }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDesscriptorHandle(ID3D12DescriptorHeap* heap,uint32_t descriptorSize , UINT index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (descriptorSize * index);
		return handle;
	}

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDesscriptorHandle(ID3D12DescriptorHeap* heap, uint32_t descriptorSize, UINT index) {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += (descriptorSize * index);
        return handle;
    }
}

int MyDirectX::ReadTexture(std::string path) {
    //TextureResourceを作成
    DirectX::ScratchImage mipImages = LoadTexture(path);
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    textureResource.push_back(CreateTextureResource(device, metadata));
    ID3D12Resource* resource = UploadTextureData(textureResource.back(), mipImages, device, commandList);
	intermediateResource.push_back(resource);

    //metadataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    //SRVを作成するDescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    textureSrvHandleGPU.push_back(srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    //先頭はImGuiが使っているのでその次を使う
    textureSrvHandleCPU = GetCPUDesscriptorHandle(srvDescriptorHeap, descriptorSizeSRV, ++readTextureCount);
    textureSrvHandleGPU.back() = GetGPUDesscriptorHandle(srvDescriptorHeap, descriptorSizeSRV, readTextureCount);

    //SRVを作成する
    device->CreateShaderResourceView(textureResource.back(), &srvDesc, textureSrvHandleCPU);

    return readTextureCount;
}

MyDirectX::MyDirectX(int32_t kWindowWidth, int32_t kWindowHeight) :
    kClientWidth(kWindowWidth),
    kClientHeight(kWindowHeight),
    clearColor(new float[4] {0.1f, 0.25f, 0.5f, 1.0f}),
    logger(new Logger("DirectX12")),
    fenceValue(0),
    alignedSize((sizeof(Matrix4x4) + 255) & ~255) {
    resourceStates[swapChainResources[0]] = D3D12_RESOURCE_STATE_PRESENT;
    resourceStates[swapChainResources[1]] = D3D12_RESOURCE_STATE_PRESENT;
}

MyDirectX::~MyDirectX() {
    delete logger;
    delete[] clearColor;
}

void MyDirectX::Initialize() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

    CreateWindowForApp();
    InitDirectX();
    InitImGui();
}

void MyDirectX::BeginFrame() {
    BeginImGui();
    ClearScreen();

    vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    wvpResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
    wvpResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite));
    vertexResourceTriangle3D->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataTriangle3D));
    wvpResourceTriangle3D->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataTriangle3D));
    materialResourceTriangle3D->Map(0, nullptr, reinterpret_cast<void**>(&materialDataTriangle3D));
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

    ShowWindow(hwnd, SW_SHOW);	    //ウィンドウを表示する
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

	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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
    rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

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

    //=======================================================

    //dxcCompilerを初期化
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    //現時点でincludeはしないが、includeに対応するための設定を行っておく
    IDxcIncludeHandler* includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

    //RootSignature作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //RootParameter作成。複数設定できるので配列
    D3D12_ROOT_PARAMETER rootParameters[3] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;     //VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//テーブルの中身
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//テーブルの数
    descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

    D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//フィルタリングの方法
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//比較しない
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;    //ありったけのMipmapを使う
    staticSampler[0].ShaderRegister = 0;    //レジスタ番号0
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
    descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ

    //シリアライズしてバイナリにする
    hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        logger->Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    //バイナリをもとに生成
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    //InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    //BlendStateの設定
    D3D12_BLEND_DESC blendDesc{};
    //全ての色要素を読み込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D12_COLOR_WRITE_ENABLE_ALL;

    //RasisterzerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    //裏面(時計回り)を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    //三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    //Shaderをコンパイルする
    vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl",
        L"vs_6_0", dxcUtils, dxcCompiler, includeHandler, logger);
    assert(vertexShaderBlob != nullptr);

    pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl",
        L"ps_6_0", dxcUtils, dxcCompiler, includeHandler, logger);
    assert(pixelShaderBlob != nullptr);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

    dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);

    device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;	//深度バッファを使う
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//全ての深度値を使う
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	//深度値の比較方法

    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    graphicsPipelineStateDesc.pRootSignature = rootSignature;
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    //書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    //利用するトロポジ(形状)のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //どのように画面に色を打ち込むかの設定
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    //実際に生成
    graphicsPipelineState = nullptr;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&graphicsPipelineState));
    assert(SUCCEEDED(hr));

    //実際に頂点リソースを作る
    vertexResourceSphere = CreateBufferResource(device, sizeof(VertexData) * 3 * 992 * 4);
    vertexResourceSphere->SetName(L"VertexResource");

    //wvpMatrixのリソース作成
    wvpResourceSphere = CreateBufferResource(device, alignedSize * 992 * 4);
    wvpResourceSphere->SetName(L"WVPResource");

    //マテリアル用のリソースを作る。今回はcolor一つ分のサイズを用意する
    materialResourceSphere = CreateBufferResource(device, sizeof(Vector4) * 992 * 4);
    materialResourceSphere->SetName(L"MaterialResource");

    //SRV用のヒープを作成する
    srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    //Sprite用の頂点リソースを作る
    vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

    //Sprite用のTransformationMatrix用のリソースを作成する
	wvpResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));

    //三角形
    vertexResourceTriangle3D = CreateBufferResource(device, sizeof(VertexData) * 3 * 1000);
    wvpResourceTriangle3D = CreateBufferResource(device, sizeof(Matrix4x4) * 1000);
    materialResourceTriangle3D = CreateBufferResource(device, sizeof(Vector4) * 1000);
}

void MyDirectX::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(device,
        2,                                              //swapchainのバッファ数
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,                //色の形式
        srvDescriptorHeap,
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );
}


void MyDirectX::ClearScreen() {
    //これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //RootSignatureを設定、PSOに設定しているけど別途設定が必要
    commandList->SetPipelineState(graphicsPipelineState);
    commandList->SetGraphicsRootSignature(rootSignature);
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

    //barrierをRenderTargetにする
    InsertBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, swapChainResources[backBufferIndex]);

    ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);

    //指定した色で画面全体をクリアする
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void MyDirectX::BeginImGui() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void MyDirectX::DrawSphere(Matrix4x4 wvpMatrix, Vector4 color, int textureHandle) {

    const float pie = 3.14159265358f;
    const int vertical = 32;
    const int horizontal = 16;

    uint32_t instantDrawTriangleCount = 0;

    //頂点データを作成する
    for (int i = 0; i < vertical; ++i) {

        int buffer;

        if (i == 0 || i == vertical - 1) {

            float point;

            if (i == 0) {
                point = 0.5f;
                buffer = 1;
            }
            else {
                point = -0.49f;
                buffer = -1;
            }

            for (int j = 0; j < horizontal; ++j) {
                //球のさきっちょ
                vertexData[drawSphereTriangleCount * 3 + 1].position = { 0.0f, point, 0.0f, 1.0f };
                vertexData[drawSphereTriangleCount * 3 + 1].texcoord = { (float(horizontal - 1 - j)) / float(horizontal - 1), buffer == 1 ? 0.0f : 0.9999f };

                //先っちょから一個離れた点たち
                vertexData[drawSphereTriangleCount * 3 + 1 - buffer].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 1 - buffer].texcoord = {
                    float(horizontal - 1 - j - 1 + (8 * buffer) - 8) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                vertexData[drawSphereTriangleCount * 3 + 1 - buffer * -1].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 1 - buffer * -1].texcoord = {
                    float(horizontal - 1 - j + (8 * buffer) - 8) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                ++drawSphereTriangleCount;
                ++instantDrawTriangleCount;
            }
        }
        else {

            if (i > vertical / 2) {
                buffer = 1;
            }
            else {
                buffer = -1;
            }

            for (int j = 0; j < horizontal; ++j) {
                //1つ目の三角形
                //RightBottom
                vertexData[drawSphereTriangleCount * 3 + 2].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 2].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                //RightTop
                vertexData[drawSphereTriangleCount * 3 + 1].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 1].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                //LeftTop
                vertexData[drawSphereTriangleCount * 3].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                ++drawSphereTriangleCount;
                ++instantDrawTriangleCount;

                //2つ目の三角形
                //RightBottom
                vertexData[drawSphereTriangleCount * 3 + 2].position = {
                    sinf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 2].texcoord = {
                    float(horizontal - 1 - j) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                //LeftTop
                vertexData[drawSphereTriangleCount * 3 + 1].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * float(i) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3 + 1].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i) / float(vertical)
                };

                //LeftBottom
                vertexData[drawSphereTriangleCount * 3].position = {
                    sinf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    cosf(pie * (j + 1) / float(horizontal - 1) * 2) * sinf(pie * float(i + 1) / float(vertical - 1)) / 2,
                    1.0f
                };
                vertexData[drawSphereTriangleCount * 3].texcoord = {
                    float(horizontal - 1 - j - 1) / float(horizontal - 1),
                    float(i + 1) / float(vertical)
                };

                ++drawSphereTriangleCount;
                ++instantDrawTriangleCount;
            }

        }
    }

    //データを書き込む
    //wvp行列を書き込む
    *wvpData = wvpMatrix;

    //データを書き込む
    //色指定
    *materialData = color;


    //頂点のバッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress() + sizeof(VertexData) * (drawSphereTriangleCount - instantDrawTriangleCount) * 3;
    //使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 3 * instantDrawTriangleCount;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    //ビューポート
    D3D12_VIEWPORT viewport{};
    //クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(kClientWidth);
    viewport.Height = static_cast<float>(kClientHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //シザー矩形
    D3D12_RECT scissorRect{};
    //基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = kClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = kClientHeight;

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    //マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResourceSphere->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResourceSphere->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    commandList->DrawInstanced(instantDrawTriangleCount * 3, 1, 0, 0);
}

void MyDirectX::DrawTriangle3D(Matrix4x4 wvpMatrix, Vector4 color, int textureHandle) {
    // 現在の描画数に基づいて三角形のベースインデックスを計算
    int baseIndex = drawTriangle3DCount * 3;

    // 三角形の頂点を設定
    vertexDataTriangle3D[baseIndex].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    vertexDataTriangle3D[baseIndex].texcoord = { 0.0f, 1.0f };

    vertexDataTriangle3D[baseIndex + 1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
    vertexDataTriangle3D[baseIndex + 1].texcoord = { 0.5f, 0.0f };

    vertexDataTriangle3D[baseIndex + 2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    vertexDataTriangle3D[baseIndex + 2].texcoord = { 1.0f, 1.0f };

    // ワールド・ビュープロジェクション行列とマテリアルカラーを書き込む
    *wvpDataTriangle3D = wvpMatrix;
    *materialDataTriangle3D = color;

    // ビューポート設定
    D3D12_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(kClientWidth);
    viewport.Height = static_cast<float>(kClientHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissorRect{};
    scissorRect.left = 0;
    scissorRect.right = kClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = kClientHeight;

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // パイプラインとルートシグネチャの設定
    commandList->SetGraphicsRootSignature(rootSignature);
    commandList->SetPipelineState(graphicsPipelineState);

    // 頂点バッファビューを更新して、これまで描画されたすべての頂点データを反映
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResourceTriangle3D->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * (baseIndex + 3);
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->SetGraphicsRootConstantBufferView(0, materialResourceTriangle3D->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, wvpResourceTriangle3D->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[textureHandle]);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 描画コールを発行
    commandList->DrawInstanced(3, 1, baseIndex, 0);

    // 描画カウントを更新
    ++drawTriangle3DCount;
}

void MyDirectX::DrawSprite(Vector4 lt, Vector4 rt, Vector4 lb, Vector4 rb, Matrix4x4 wvpMatrix, Vector4 color) {
    //1つ目の三角形
    vertexDataSprite[0].position = lb;
    vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
    
    vertexDataSprite[1].position = lt;
    vertexDataSprite[1].texcoord = { 0.0f, 0.0f };

    vertexDataSprite[2].position = rb;
    vertexDataSprite[2].texcoord = { 1.0f, 1.0f };

    //2つ目の三角形
    vertexDataSprite[3].position = lt;
    vertexDataSprite[3].texcoord = { 0.0f, 0.0f };

    vertexDataSprite[4].position = rt;
    vertexDataSprite[4].texcoord = { 1.0f, 0.0f };

    vertexDataSprite[5].position = rb;
    vertexDataSprite[5].texcoord = { 1.0f, 1.0f };

    *wvpDataSprite = wvpMatrix;

    //頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
    //リソースの先頭のアドレスから使う
    vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点6つ分のサイズ
    vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
    //1頂点当たりのサイズ
    vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

    //Spriteの描画。変更が必要なものだけ変更する
    commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
    //TransformationMatrixCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
    //Texture
    commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
    //描画
    commandList->DrawInstanced(6, 1, 0, 0);
}

void MyDirectX::EndFrame() {

    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    InsertBarrier(commandList, D3D12_RESOURCE_STATE_PRESENT, swapChainResources[backBufferIndex]);

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

    drawTriangle2DCount = 0;
    drawSphereTriangleCount = 0;
    drawTriangle3DCount = 0;
    //次のフレームのためのcommandReset
    commandAllocator->Reset();
    commandList->Reset(commandAllocator, nullptr);
}

void MyDirectX::Finalize() {
    vertexResourceTriangle3D->Release();
    wvpResourceTriangle3D->Release();
	materialResourceTriangle3D->Release();
	vertexResourceSprite->Release();
	wvpResourceSprite->Release();
    dsvDescriptorHeap->Release();
    depthStencilResource->Release();
    for (uint32_t i = 0; i < readTextureCount; ++i) {
        intermediateResource[i]->Release();
        textureResource[i]->Release();
    }
    vertexResourceSphere->Release();
    wvpResourceSphere->Release();
    materialResourceSphere->Release();
    graphicsPipelineState->Release();
    signatureBlob->Release();
    if (errorBlob) {
        errorBlob->Release();
    }
    rootSignature->Release();
    pixelShaderBlob->Release();
    vertexShaderBlob->Release();
    CloseHandle(fenceEvent);
    fence->Release();
    srvDescriptorHeap->Release();
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

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    IDXGIDebug* debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }

    CoUninitialize();
}

void MyDirectX::InsertBarrier(ID3D12GraphicsCommandList* commandlist, D3D12_RESOURCE_STATES stateAfter, ID3D12Resource* pResource,
    D3D12_RESOURCE_BARRIER_TYPE type, D3D12_RESOURCE_BARRIER_FLAGS flags) {

    D3D12_RESOURCE_STATES stateBefore = resourceStates[pResource];

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = type;
    barrier.Flags = flags;
    barrier.Transition.pResource = pResource;
    barrier.Transition.StateBefore = stateBefore;
    barrier.Transition.StateAfter = stateAfter;

    commandList->ResourceBarrier(1, &barrier);

    // 状態を更新
    resourceStates[pResource] = stateAfter;
}
