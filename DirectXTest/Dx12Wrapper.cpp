#include "Dx12Wrapper.h"

#include <cassert>
#include <d3dx12.h>

#include "Singleton.h"
#include "Device.h"

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

namespace {
    //  モデルのパスとテクスチャのパスから合成パスを得る
    //  @param modelPath アプリケーションから見たpmdモデルのパス
    //  @param texPath PMD モデルから見たテクスチャのパス
    //  @return アプリケーションから見たテクスチャのパス
    std::string GetTexturePathFromModelAndTexPath(
        const std::string& _modelPath,
        const char* _texPath)
    {
        //  ファイルのフォルダー区切りは\と/の2種類が使用される可能性があり
        //  ともかく末尾の\か/を得られればいいので、双方のrfindを取り比較する
        //  （int型に代入しているのは、見つからなかった場合はrfindがepos(-1 → 0xffffffff)を返すため
        int pathIndex1 = _modelPath.rfind('/');
        int pathIndex2 = _modelPath.rfind('\\');

        auto pathIndex = max(pathIndex1, pathIndex2);
        auto folderPath = _modelPath.substr(0, pathIndex + 1);

        return folderPath + _texPath;
    }

    //  ファイル名から拡張子を取得する
    //  @param path 対象のパス文字列
    //  @return 拡張子
    std::string GetExtension(const std::string& _path)
    {
        int idx = _path.rfind('.');
        return _path.substr(idx + 1, _path.length() - idx - 1);
    }

    ///  <summary>
    ///  テクスチャのパスをセパレータ文字で分離する
    ///  </summary>
    ///  @param path 大賞のパス文字列
    ///  @param splitter 区切り文字
    ///  @return 分離前後の文字列ペア
    std::wstring GetExtension(const std::wstring& _path)
    {
        int idx = _path.rfind(L'.');
        return _path.substr(idx + 1, _path.length() - idx - 1);
    }

    //  テクスチャのパスをセパレーター文字で分離する
    //  @param path 対象のパス文字列
    //  @param splitter 区切り文字
    //  @return 分離前後の文字列ペア
    std::pair<std::string, std::string> SplitFileName(const std::string & _path, const char _splitter = '*')
    {
        int idx = _path.find(_splitter);
        std::pair<std::string, std::string> ret;
        ret.first = _path.substr(0, idx);
        ret.second = _path.substr(idx + 1, _path.length() - idx - 1);
        return ret;
    }

    //  std::string（マルチバイト文字列）からstd::wstring（ワイド文字列）を得る
    //  @param str マルチバイト文字列
    //  @return 変換されたワイド文字列
    std::wstring GetWideStringFromString(const std::string& _str)
    {
        //  呼び出し1回目（文字列数を得る）
        auto num1 = MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            _str.c_str(),
            -1,
            nullptr,
            0);

        std::wstring wstr;         //  stringのwchar_t版
        wstr.resize(num1);    //  得られた文字列数でリサイズ

        //  呼び出し2回目（確保済みのwstrに変換文字列をコピー）
        auto num2 = MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            _str.c_str(),
            -1,
            &wstr[0],
            num1);

        assert(num1 == num2);    //  一応チェック
        return wstr;
    }

    //  デバッグレイヤーを有効にする
    void EnableDebugLayer()
    {
        ComPtr<ID3D12Debug> debugLayer = nullptr;
        auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

        debugLayer->EnableDebugLayer();
    }
}

Dx12Wrapper::Dx12Wrapper(HWND _hwnd)
{
#ifdef _DEBUG
    //  デバッグレイヤーをオンに
    EnableDebugLayer();
#endif

    Device& device = Singleton<Device>::GetInstance();
    m_winSize = device.GetWindowSize();

    //  DirectX12関連初期化
    if (FAILED(InitializeDXGIDevice())) {
        assert(0);
        return;
    }
    if (FAILED(InitializeCommand())) {
        assert(0);
        return;
    }
    if (FAILED(CreateSwapChain(_hwnd))) {
        assert(0);
        return;
    }
    if (FAILED(CreateFinalRenderTargets())) {
        assert(0);
        return;
    }

    if (FAILED(CreateSceneView())) {
        assert(0);
        return;
    } 

    //  TextureLoader関連初期化
    CreateTextureLoaderTable();

    //  DepthStencilView作成
    if (FAILED(CreateDepthStencilView())) 
    {
        assert(0);
        return;
    }
    //  Fenceの作成
    if (FAILED(m_dev->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())))) {
        assert(0);
        return;
    }
}

//  DepthStencilViewの生成
HRESULT Dx12Wrapper::CreateDepthStencilView()
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    auto result = m_swapchain->GetDesc1(&desc);

    //  深度バッファの設定
    D3D12_RESOURCE_DESC resdesc = {};
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resdesc.DepthOrArraySize = 1;
    resdesc.Width = desc.Width;
    resdesc.Height = desc.Height;
    resdesc.Format = DXGI_FORMAT_D32_FLOAT;
    resdesc.SampleDesc.Count = 1;
    resdesc.SampleDesc.Quality = 0;
    resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resdesc.MipLevels = 1;
    resdesc.Alignment = 0;

    //  深度用HeapProperty
    auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    //  深度バッファの作成
    CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
    result = m_dev->CreateCommittedResource(
        &depthHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &resdesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,    //  深度書き込みに使用
        &depthClearValue,
        IID_PPV_ARGS(m_depthBuffer.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        //  エラー処理
        return result;
    }

    //  深度のためのDescriptorHeap作成
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};    
    dsvHeapDesc.NumDescriptors = 1;    //  DepthStencilView1つのみ
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;    //  DepthStencilViewとして使う
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


    result = m_dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf()));

    //  DepthStencilView作成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;    //  深度値に32bit使用
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;    //  2Dテクスチャ
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;    //  フラグは特になし
    m_dev->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

//  SwapChainの生成
HRESULT Dx12Wrapper::CreateSwapChain(const HWND& _hwnd)
{
    RECT rc = {};
    ::GetWindowRect(_hwnd, &rc);

    //  SwapChainの設定
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
    swapchainDesc.Width = m_winSize.cx;
    swapchainDesc.Height = m_winSize.cy;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = false;
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.BufferCount = 2;
    swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    //  フルスクリーン設定
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
    fsDesc.Windowed = false;    //  フルスクリーンにする
    fsDesc.RefreshRate.Denominator = 1;
    fsDesc.RefreshRate.Numerator = 60;
    fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    //  SwapChainの作成
    auto result = m_dxgiFactory->CreateSwapChainForHwnd(m_cmdQueue.Get(),
        _hwnd,
        &swapchainDesc,
        &fsDesc,
        nullptr,
        (IDXGISwapChain1**)m_swapchain.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(result));

    return result;
}

//  DXGIまわり初期化
HRESULT Dx12Wrapper::InitializeDXGIDevice()
{
    UINT flagsDXGI = 0;
#if _DEBUG
    flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf()));
    //  DirectX12まわり初期化
    //  FeatureLevel列挙
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    if (FAILED(result)) {
        return result;
    }
    std::vector <IDXGIAdapter*> adapters;
    IDXGIAdapter* tmpAdapter = nullptr;
    for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        adapters.push_back(tmpAdapter);
    }
    for (auto adpt : adapters) {
        DXGI_ADAPTER_DESC adesc = {};
        adpt->GetDesc(&adesc);
        std::wstring strDesc = adesc.Description;
        if (strDesc.find(L"NVIDIA") != std::string::npos) {
            tmpAdapter = adpt;
            break;
        }
    }
    result = S_FALSE;
    //  Direct3Dデバイスの初期化
    D3D_FEATURE_LEVEL featureLevel;
    for (auto l : levels) {
        if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(m_dev.ReleaseAndGetAddressOf())))) {
            featureLevel = l;
            result = S_OK;
            break;
        }
    }
    return result;
}

Dx12Wrapper::~Dx12Wrapper()
{
    //  処理なし
}

//  描画の開始
void Dx12Wrapper::BeginDraw()
{
    //  DirectX処理
    //  BackBufferIndexを取得
    auto bbIdx = m_swapchain->GetCurrentBackBufferIndex();
    auto temp = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_cmdList->ResourceBarrier(1,&temp);


    //  RenderTargetを指定
    auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    rtvH.ptr += bbIdx * m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //  深度を指定
    auto dsvH = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    m_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
    m_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


    //  画面クリア
    float clearColor[] = { 1.0f,1.0f,1.0f,1.0f };    //  白色
    m_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

    //  Viewport、ScissorRectのセット
    m_cmdList->RSSetViewports(1, m_viewport.get());
    m_cmdList->RSSetScissorRects(1, m_scissorrect.get());
}

//  描画の終了
void Dx12Wrapper::EndDraw()
{
    auto bbIdx = m_swapchain->GetCurrentBackBufferIndex();
    auto temp = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_cmdList->ResourceBarrier(1,&temp);

    //  命令のクローズ
    m_cmdList->Close();

    //  CommandListの実行
    ID3D12CommandList* cmdlists[] = { m_cmdList.Get() };
    m_cmdQueue->ExecuteCommandLists(1, cmdlists);
    //  待ち
    m_cmdQueue->Signal(m_fence.Get(), ++m_fenceVal);

    //  GPUの処理完了確認
    if (m_fence->GetCompletedValue() < m_fenceVal) {
        auto event = CreateEvent(nullptr, false, false, nullptr);
        m_fence->SetEventOnCompletion(m_fenceVal, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
    //  CommandQueをクリア
    m_cmdAllocator->Reset();
    //  再びCommandListをためる準備
    m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
}

//  テクスチャパスから必要なテクスチャバッファへのポインタを返す
ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const char* _texpath)
{
    auto it = m_textureTable.find(_texpath);
    if (it != m_textureTable.end()) {
        //  テーブルに内にあったらロードするのではなくマップ内の
        //  リソースを返す
        return m_textureTable[_texpath];
    }
    else {
        return ComPtr<ID3D12Resource>(CreateTextureFromFile(_texpath));
    }
}

//  ViewPortを返す
D3D12_VIEWPORT Dx12Wrapper::GetViewPort() const
{
    Device& device = Singleton<Device>::GetInstance();
    auto wsize = device.GetWindowSize();
    D3D12_VIEWPORT vp;
    vp.TopLeftX = vp.TopLeftY = 0;
    vp.Width = wsize.cx;
    vp.Height = wsize.cy;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    return vp;
}

//  SpriteFont用のDescriptorHeapを返す
ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::CreateDescriptorHeapForSpriteFont()
{
    return CreateOneDescriptorHeapSRV();
}

//  CommandQueueの処理待ち
void Dx12Wrapper::WaitForCommandQueue()
{
    //仮に初回として_fenceValueが0だったとします
    m_cmdQueue->Signal(m_fence.Get(), ++m_fenceVal);
    //↑の命令直後では_fenceValueは1で、
    //GetCompletedValueはまだ0です。
    if (m_fence->GetCompletedValue() < m_fenceVal) {
        //もしまだ終わってないなら、イベント待ちを行う
        //↓そのためのイベント？あとそのための_fenceValue
        auto event = CreateEvent(nullptr, false, false, nullptr);
        //フェンスに対して、CompletedValueが_fenceValueに
        //なったら指定のイベントを発生させるという命令↓
        m_fence->SetEventOnCompletion(m_fenceVal, event);
        //↑まだイベント発生しない
        //↓イベントが発生するまで待つ
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

//  シーン(ビュープロジェクション)をセット
void Dx12Wrapper::SetScene()
{
    //  現在のシーン(ビュープロジェクション)をセット
    ID3D12DescriptorHeap* sceneheaps[] = { m_sceneDescHeap.Get() };
    m_cmdList->SetDescriptorHeaps(1, sceneheaps);
    m_cmdList->SetGraphicsRootDescriptorTable(0, m_sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

//  カメラの座標を更新
void Dx12Wrapper::UpdateCameraPos(const XMFLOAT3& _eye, const XMFLOAT3& _target, const XMFLOAT3& _up)
{
    //  指定位置にカメラを移動
    m_mappedSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
}

//  FinalRenderTargetの生成
HRESULT Dx12Wrapper::CreateFinalRenderTargets()
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    auto result = m_swapchain->GetDesc1(&desc);

    //  RenderTarget用のDescriptorHeapの作成
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;    //  RenderTargetViewなので当然RTV
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 2;    //  表裏の２つ
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;    //  特に指定なし
    result = m_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_rtvHeaps.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        SUCCEEDED(result);
        return result;
    }

    //  SwapChainの設定
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    result = m_swapchain->GetDesc(&swcDesc);
    m_backBuffers.resize(swcDesc.BufferCount);

    //  DescriptorHeapのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

    //  SRGBRenderTargetView設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    //  RenderTargetViewの生成
    for (int i = 0; i < swcDesc.BufferCount; i++) {
        result = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
        assert(SUCCEEDED(result));
        rtvDesc.Format = m_backBuffers[i]->GetDesc().Format;
        m_dev->CreateRenderTargetView(m_backBuffers[i], &rtvDesc, handle);
        handle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    m_viewport.reset(new CD3DX12_VIEWPORT(m_backBuffers[0]));
    m_scissorrect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
    return result;
}

//  Commandまわり初期化
HRESULT Dx12Wrapper::InitializeCommand()
{
    auto result = m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        assert(0);
        return result;
    }
    result = m_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        assert(0);
        return result;
    }

    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; //  タイムアウトなし
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;    //  Priority特に指定なし
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //  ここはCommandListと合わせてください
    result = m_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_cmdQueue.ReleaseAndGetAddressOf()));   //  CommandQueue生成
    assert(SUCCEEDED(result));
    return result;
}

//  ビュープロジェクション用ビューの生成
HRESULT Dx12Wrapper::CreateSceneView()
{
    //  SwapChainの設定
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    auto result = m_swapchain->GetDesc1(&desc);

    //  定数バッファ作成
    auto temp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto temp2 = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);
    result = m_dev->CreateCommittedResource(
        &temp1,
        D3D12_HEAP_FLAG_NONE,
        &temp2,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_sceneConstBuff.ReleaseAndGetAddressOf()));

    if (FAILED(result)) 
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  マップ先を示すポインタ
    m_mappedSceneData = nullptr;
    //  マップ
    result = m_sceneConstBuff->Map(0, nullptr, (void**)&m_mappedSceneData);

    //  カメラの設定
    XMFLOAT3 eye(0, 60, -100);
    XMFLOAT3 target(0,0,0);
    XMFLOAT3 up(0, 1, 0);
    m_mappedSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
    m_mappedSceneData->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,    //  画角は45°
        static_cast<float>(desc.Width) / static_cast<float>(desc.Height),    //  アス比
        0.1f,    //  近い方
        1000.0f    //  遠い方
    );
    m_mappedSceneData->eye = eye;

    //  DescriptorHeapの設定
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;    //  シェーダから見えるように
    descHeapDesc.NodeMask = 0;    //  マスクは0
    descHeapDesc.NumDescriptors = 1;//
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;    //  デスクリプタヒープ種別
    result = m_dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(m_sceneDescHeap.ReleaseAndGetAddressOf()));//生成

    //  Descriptorの先頭ハンドルを取得しておく
    auto heapHandle = m_sceneDescHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_sceneConstBuff->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = m_sceneConstBuff->GetDesc().Width;
    //  定数バッファビューの作成
    m_dev->CreateConstantBufferView(&cbvDesc, heapHandle);
    return result;
}

//  TextureLoaderTableの作成
void Dx12Wrapper::CreateTextureLoaderTable()
{
    m_loadLambdaTable["sph"] = m_loadLambdaTable["spa"] = m_loadLambdaTable["bmp"] = m_loadLambdaTable["png"] = m_loadLambdaTable["jpg"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromWICFile(_path.c_str(), WIC_FLAGS_NONE, _meta, _img);
    };

    m_loadLambdaTable["tga"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromTGAFile(_path.c_str(),_meta, _img);
    };

    m_loadLambdaTable["dds"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromDDSFile(_path.c_str(), DDS_FLAGS_NONE, _meta, _img);
    };
}

//  テクスチャ名からテクスチャバッファ作成、中身をコピー
ID3D12Resource* Dx12Wrapper::CreateTextureFromFile(const char* _texpath)
{
    std::string texPath = _texpath;
    //  テクスチャのロード
    TexMetadata metadata = {};
    ScratchImage scratchImg = {};
    auto wtexpath = GetWideStringFromString(texPath);    //  テクスチャのファイルパス
    auto ext = GetExtension(texPath);    //  拡張子を取得
    auto result = m_loadLambdaTable[ext](wtexpath,
        &metadata,
        scratchImg);
    if (FAILED(result)) {
        return nullptr;
    }
    auto img = scratchImg.GetImage(0, 0, 0);    //  生データ抽出

    //  WriteToSubresourceで転送する用のヒープ設定
    auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

    //  テクスチャ用バッファの作成
    ID3D12Resource* texbuff = nullptr;
    result = m_dev->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,    //  特に指定なし
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&texbuff)
    );

    if (FAILED(result)) {
        return nullptr;
    }
    result = texbuff->WriteToSubresource(0,
        nullptr,        //  全領域へコピー
        img->pixels,    //  元データアドレス
        img->rowPitch,    //  1ラインサイズ
        img->slicePitch    //  全サイズ
    );
    if (FAILED(result)) {
        return nullptr;
    }

    return texbuff;
}

ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::CreateOneDescriptorHeapSRV()
{
    ComPtr<ID3D12DescriptorHeap> ret;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    m_dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
    return ret;
}
