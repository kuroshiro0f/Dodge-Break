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

    //  DebugLayerを有効にする
    //  NOTE: DXGIのエラー情報を取得するためのインターフェース
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
    //  DebugLayerをオンに
    EnableDebugLayer();
#endif
    //  Deviceクラスのシングルトンインスタンスを取得
    Device& device = Singleton<Device>::GetInstance();
    //  ウィンドウサイズを取得
    m_windowSize = device.GetWindowSize();

    //  DirectX12用にGPUデバイスを初期化
    if (FAILED(InitializeDXGIDevice())) {
        assert(0);
        return;
    }
    //  CommandList関連の作成
    if (FAILED(InitializeCommand())) {
        assert(0);
        return;
    }
    //  SwapChainの作成
    if (FAILED(CreateSwapChain(_hwnd))) {
        assert(0);
        return;
    }
    //  RenderTargetを作成
    if (FAILED(CreateRenderTargets())) {
        assert(0);
        return;
    }
    //  カメラを作成
    if (FAILED(CreateCamera())) {
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

    //  DepthStencilBufferの設定
    D3D12_RESOURCE_DESC resdesc = {};
    resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;    //  DepthStencilBufferとしてResource情報を設定
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //  テクスチャのデータは2D
    resdesc.DepthOrArraySize = 1;       //  テクスチャ配列や3Dテクスチャではない
    resdesc.Width = desc.Width;         //  幅はSwapChainの設定に合わせる
    resdesc.Height = desc.Height;       //  高さもSwapChainの設定に合わせる
    resdesc.Format = DXGI_FORMAT_D32_FLOAT;     //  今回はDepthStencilBufferの32ビット中全て深度値に使用(ステンシル値は使用しない)
    resdesc.SampleDesc.Count = 1;       //  1ピクセル当たりのマルチサンプル数は1つ
    resdesc.SampleDesc.Quality = 0;     //  Qualityを最低にし、パフォーマンスを優先する
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;  //  テクスチャのレイアウトはAdapterに依存
    resdesc.MipLevels = 1;
    resdesc.Alignment = 0;

    //  深度用HeapPropertyをデフォルトの値で設定
    auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    //  深度値のクリアに使用するDepthClearValueを設定
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

    //  DepthStencilViewの情報をDiscriptorとしてGPUに渡すための領域情報(DescriptorHeap)を設定
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};    
    dsvHeapDesc.NumDescriptors = 1;    //  DepthStencilView1つのみ
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;      //  DepthStencilViewとして使う
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;    //  使用法はデフォルトで
    //  DepthStencilView用のDescriporHeap作成
    result = m_dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.ReleaseAndGetAddressOf()));

    //  DepthStencilViewの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;    //  今回はDepthStencilViewの32ビット中全て深度値に使用(ステンシル値は使用しない)
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;    //  2Dテクスチャ
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;        //  使用法はデフォルトで
    //  DepthStencilView作成
    m_dev->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

//  SwapChainの作成
//  NOTE: ダブルバッファリングを使用するために、
//        表示する画面(表画面)と描画する画面(裏画面)を定義してFlipする(切り替える)ための仕組み
HRESULT Dx12Wrapper::CreateSwapChain(const HWND& _hwnd)
{
    //  SwapChainの設定
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
    swapchainDesc.Width = m_windowSize.cx;     //  幅はウィンドウに合わせる
    swapchainDesc.Height = m_windowSize.cy;    //  高さもウィンドウに合わせる
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //  表示形式はRGBAの32ビットの符号なし正規化整数形式
    swapchainDesc.Stereo = false;   //  Stereo(立体視)は使用しない
    swapchainDesc.SampleDesc.Count = 1;       //  1ピクセル当たりのマルチサンプル数は1つ
    swapchainDesc.SampleDesc.Quality = 0;     //  Qualityを最低にし、パフォーマンスを優先する
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    //  SwapChainの処理はウィンドウに出力
    swapchainDesc.BufferCount = 2;      //  表画面と裏画面の2枚
    swapchainDesc.Scaling = DXGI_SCALING_STRETCH;   //  裏画面をウィンドウに出力する際にサイズを合わせる
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    //  Flip後に裏画面の内容を保持する
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;      //  AlphaModeの指定はなし
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   //  ウィンドウモードとフルスクリーンモードの切り替えに対応

    //  フルスクリーン設定
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
    fsDesc.Windowed = false;    //  フルスクリーンにする
    fsDesc.RefreshRate.Denominator = 1;     //  RefreshRateは1～60
    fsDesc.RefreshRate.Numerator = 60;
    fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;     //  Scalingに指定なし
    fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     //  Scanlineの順序に指定なし

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

//  Direct3DDevice作成
HRESULT Dx12Wrapper::InitializeDXGIDevice()
{
    //  DXGIFactoryのフラグを宣言
    UINT flagsDXGI = 0;
#if _DEBUG
    //  Debag時にはDXGIDebug.dllを読み込む
    flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    //  DXGIFactory作成
    auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf()));
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

    //  使用可能なAdapterを配列に格納
    std::vector <IDXGIAdapter*> adapters;
    IDXGIAdapter* tmpAdapter = nullptr;
    for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        adapters.push_back(tmpAdapter);
    }
    //  各Adapterの情報を取得し、その中からNVIDIAが名前に含まれているAdapterを検索
    //  NOTE: NVIDIAがAdapterとして存在しない場合はデフォルトのAdapterを使用
    for (auto adptItr : adapters) {
        DXGI_ADAPTER_DESC adesc = {};
        adptItr->GetDesc(&adesc);
        std::wstring strDesc = adesc.Description;
        if (strDesc.find(L"NVIDIA") != std::string::npos) {
            tmpAdapter = adptItr;
            break;
        }
    }
    result = S_FALSE;

    //  Direct3DDeviceの生成
    //  NOTE: Adapterに対応しているFeatureLevelを検索して、対応しているものがあればDeviceを生成する
    D3D_FEATURE_LEVEL featureLevel;
    for (auto levelItr : levels) {
        if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, levelItr, IID_PPV_ARGS(m_dev.ReleaseAndGetAddressOf())))) {
            featureLevel = levelItr;
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
    //  ダブルバッファリングに使用する画面2つのうち、裏画面にしている画面を指定する配列の添え字を取得
    auto bbIdx = m_swapchain->GetCurrentBackBufferIndex();
    //  Resourceの使用状態を画面のFlipから書き込みに遷移(ResourceBarrier)
    auto tempBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[bbIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_cmdList->ResourceBarrier(1,&tempBarrier);

    //  RenderTargetの指定先を現在の裏画面に設定
    auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    rtvH.ptr += bbIdx * m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //  深度を設定
    auto dsvH = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    //  RenderTargetを設定
    m_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
    //  書き込まれている深度値をクリア
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
    //  ダブルバッファリングに使用する画面2つのうち、裏画面にしている画面を指定する配列の添え字を取得
    auto backBufferIndex = m_swapchain->GetCurrentBackBufferIndex();
    //  Resourceの使用状態を書き込みから画面のFlipに遷移(ResourceBarrier)
    auto temp = CD3DX12_RESOURCE_BARRIER::Transition(m_backBuffers[backBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_cmdList->ResourceBarrier(1,&temp);

    //  GPUへの命令の発行を閉じる
    //  NOTE: GPUへの命令の書き込みと命令の実行は同時に行えないため、
    //        命令の実行前に書き込みを終了させる
    m_cmdList->Close();

    //  CommandAllocatorに溜められたGPUへの処理命令を実行
    ID3D12CommandList* cmdlists[] = { m_cmdList.Get() };
    m_cmdQueue->ExecuteCommandLists(1, cmdlists);

    //  CommandQueueの処理待ち
    WaitForCommandQueue();

    //  CommandAllocatorおよびCommandQueueをクリア
    m_cmdAllocator->Reset();
    //  再びCommadListを溜めるためにCommadListを使用して命令を溜めるCommandAllocatorの指定と、
    //  Close関数によって閉じた命令の発行を再開させる処理
    m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
}

//  Textureのパスから必要なTextureBufferへのポインタを返す
ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const char* _texpath)
{
    //  すでにロード済のTextureの場合は、その情報を使いまわす
    auto itr = m_textureTable.find(_texpath);
    if (itr != m_textureTable.end()) {
        return m_textureTable[_texpath];
    }
    //  まだロードしていないTextureの場合はTextureファイルから情報を取得
    else {
        return ComPtr<ID3D12Resource>(CreateTextureFromFile(_texpath));
    }
}

//  SpriteFont用のDescriptorHeapを返す
ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::CreateDescriptorHeapForSpriteFont()
{
    ComPtr<ID3D12DescriptorHeap> retDiscriptorHeap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;     //  シェーダーから見えるように
    desc.NodeMask = 0;      //  操作するAdoptorは単一
    desc.NumDescriptors = 1;    //  Discriptorの数は1つ
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;     //  定数Buffer、ShaderResource、順序なしAccessのどれかのView(今回はShaderResource)
    //  DiscriptorHeapの作成
    m_dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(retDiscriptorHeap.ReleaseAndGetAddressOf()));
    return retDiscriptorHeap;
}

//  CommandQueueの処理待ち
void Dx12Wrapper::WaitForCommandQueue()
{
    //  GPUの処理が完了しているか確認
    //  NOTE: GPUの処理が終わっている場合のみFence内の値がインクリメントされ、
    //        m_fenceValは処理が終わっていない場合でも値がインクリメントされる
    m_cmdQueue->Signal(m_fence.Get(), ++m_fenceVal);
    //  GPUの処理が全て完了しているか確認
    //  NOTE: Fence内の値がm_fenceValの値より小さい場合は、GPUの処理が終わっていないということになるため、
    //        その場合は空のイベントを発行してGPUの処理が終了するまで待つ
    if (m_fence->GetCompletedValue() < m_fenceVal) {
        auto event = CreateEvent(nullptr, false, false, nullptr);
        m_fence->SetEventOnCompletion(m_fenceVal, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

//  カメラをセット
void Dx12Wrapper::SetScene()
{
    ID3D12DescriptorHeap* cameraheaps[] = { m_cameraDescHeap.Get() };
    m_cmdList->SetDescriptorHeaps(1, cameraheaps);
    m_cmdList->SetGraphicsRootDescriptorTable(0, m_cameraDescHeap->GetGPUDescriptorHandleForHeapStart());
}

//  カメラの座標を更新
void Dx12Wrapper::UpdateCameraPos(const XMFLOAT3& _eye, const XMFLOAT3& _target, const XMFLOAT3& _up)
{
    //  指定位置にカメラを移動
    m_mappedCameraData->view = XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
}

//  RenderTargetの生成
HRESULT Dx12Wrapper::CreateRenderTargets()
{
    //  出力する画面の情報を取得
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    auto result = m_swapchain->GetDesc1(&desc);

    //  RenderTarget用のDescriptorHeapの設定
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;    //  RenderTargetViewをTypeに指定
    heapDesc.NodeMask = 0;      //  操作するAdaptorは1つのみ
    heapDesc.NumDescriptors = 2;    //  表画面と裏画面の2つ(厳密には裏画面にのみ描画するため、裏画面になることがある画面が2つという意味)
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;    //  特に指定なし
    //  RenderTarget用のDescriptorHeapの作成
    result = m_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_rtvHeaps.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        SUCCEEDED(result);
        return result;
    }

    //  裏画面のResize
    //  NOTE: RenderTargetの設定前に裏画面のSizeを設定しておく
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    result = m_swapchain->GetDesc(&swcDesc);
    m_backBuffers.resize(swcDesc.BufferCount);

    //  DescriptorHeapの先頭アドレスを取得
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

    //  SRGBRenderTargetView設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;   //  表示形式はAlphaを含むSRGBの32ビットの符号なし正規化整数形式
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  //  RenderTargetViewを使用してアクセスできるResourceのTextureは2DTexture

    //  画面の数だけRenderTargetViewを作成
    for (int i = 0; i < swcDesc.BufferCount; i++) {
        //  画面を指定
        result = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
        assert(SUCCEEDED(result));
        //  Formatは画面に合わせる
        rtvDesc.Format = m_backBuffers[i]->GetDesc().Format;
        //  RenderTargetViewの生成
        m_dev->CreateRenderTargetView(m_backBuffers[i], &rtvDesc, handle);
        //  DescriporHeap内でDiscriptorを作成する先頭アドレスの指定先を作成したRenderTargetViewのサイズ分ずらす
        handle.ptr += m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    //  ViewPortとScissorRectをウィンドウに合わせる
    m_viewport.reset(new CD3DX12_VIEWPORT(m_backBuffers[0]));
    m_scissorrect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
    return result;
}

//  CommandList関連の作成
HRESULT Dx12Wrapper::InitializeCommand()
{
    //  CommandAllocatorを作成
    auto result = m_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        assert(0);
        return result;
    }
    //  CommandListを作成
    result = m_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        assert(0);
        return result;
    }

    //  CommandQueueを作成
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;     //  GPUの処理のタイムアウトは無効にしない
    cmdQueueDesc.NodeMask = 0;      //  単一のGPUを操作
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;    //  デフォルトの設定を使用
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //  CommadListに合わせて設定
    //  CommandQueue生成
    result = m_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_cmdQueue.ReleaseAndGetAddressOf()));
    assert(SUCCEEDED(result));
    return result;
}

//  カメラの作成
HRESULT Dx12Wrapper::CreateCamera()
{
    //  出力する画面の情報を取得
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    auto result = m_swapchain->GetDesc1(&desc);

    //  カメラを構成する情報を設定する定数Bufferを作成
    auto tempHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto tempResourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(CameraData) + 0xff) & ~0xff);
    result = m_dev->CreateCommittedResource(
        &tempHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &tempResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_cameraConstBuff.ReleaseAndGetAddressOf()));
    if (FAILED(result)) 
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  カメラの情報を更新するためのポインタを空に
    m_mappedCameraData = nullptr;
    //  BufferをGPUにアップロードして、その情報を書き換えるための仮想ポインタをm_mappedCameraDataに格納(Map)
    //  NOTE: Unmapを行わず、常に書き換えが可能な状態にする
    result = m_cameraConstBuff->Map(0, nullptr, (void**)&m_mappedCameraData);

    //  カメラの設定
    //  視線ベクトル
    //  NOTE: ユーザーの視線をベクトル化したもの
    XMFLOAT3 eye(0, 60, -100);
    //  注視点ベクトル
    //  NOTE: 視点の中心となるばしょ
    XMFLOAT3 target(0,0,0);
    //  上ベクトル
    //  NOTE: どの方向をカメラの上とするか
    XMFLOAT3 up(0, 1, 0);
    //  ビュー行列の設定
    m_mappedCameraData->view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
    //  プロジェクション行列の設定
    m_mappedCameraData->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,    //  視野角は45°
        static_cast<float>(desc.Width) / static_cast<float>(desc.Height),    //  アスペクト比
        0.1f,   //  カメラが写す一番近い距離(Near)
        1000.0f     //  カメラが写す一番遠い距離(Far)
    );
    //  視線ベクトルの設定
    m_mappedCameraData->eye = eye;

    //  カメラ用定数BufferViewのDescriptorHeapの設定
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;    //  シェーダから見えるように
    descHeapDesc.NodeMask = 0;    //  操作するAdoptorは単一
    descHeapDesc.NumDescriptors = 1;    //  Discriptorの数は1つ(カメラは1つ)
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;    //  定数Buffer、ShaderResource、順序なしAccessのどれかのView(今回は定数Buffer)
    //  カメラ用定数BufferViewのDescriporHeapの生成
    result = m_dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(m_cameraDescHeap.ReleaseAndGetAddressOf()));

    //  カメラ用定数BufferViewのDescriptorHeapの先頭ハンドルを取得しておく
    auto heapHandle = m_cameraDescHeap->GetCPUDescriptorHandleForHeapStart();

    //  カメラ用定数BufferViewの設定
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_cameraConstBuff->GetGPUVirtualAddress();      //  カメラ用定数BufferのViewに設定
    cbvDesc.SizeInBytes = m_cameraConstBuff->GetDesc().Width;    //  定数Bufferのサイズを設定
    //  カメラ用定数BufferViewの作成
    m_dev->CreateConstantBufferView(&cbvDesc, heapHandle);

    return result;
}

//  TextureLoaderTableの作成
//  NOTE: Textureのファイルパスの記述子の種類によって使用する関数を分ける
void Dx12Wrapper::CreateTextureLoaderTable()
{
    //  記述子が.sph、.spa、.bmp、.png、.jpgのいずれかの場合
    m_loadLambdaTable["sph"] = m_loadLambdaTable["spa"] = m_loadLambdaTable["bmp"] = m_loadLambdaTable["png"] = m_loadLambdaTable["jpg"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromWICFile(_path.c_str(), WIC_FLAGS_NONE, _meta, _img);
    };
    //  記述子が.tgaの場合
    m_loadLambdaTable["tga"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromTGAFile(_path.c_str(),_meta, _img);
    };
    //  記述子が.ddsの場合
    m_loadLambdaTable["dds"] = [](const std::wstring& _path, TexMetadata* _meta, ScratchImage& _img)->HRESULT {
        return LoadFromDDSFile(_path.c_str(), DDS_FLAGS_NONE, _meta, _img);
    };
}

//  Texture名からTextureBuffer作成
ID3D12Resource* Dx12Wrapper::CreateTextureFromFile(const char* _texpath)
{
    //  Textureのファイルパスをstring型に変換
    std::string texPath = _texpath;

    //  Textureのロード
    TexMetadata metadata = {};
    ScratchImage scratchImg = {};
    auto wtexpath = GetWideStringFromString(texPath);    //  Textureのファイルパス
    auto ext = GetExtension(texPath);    //  拡張子を取得
    //  Textureの情報を取得
    auto result = m_loadLambdaTable[ext](wtexpath,
        &metadata,
        scratchImg);
    if (FAILED(result)) {
        return nullptr;
    }

    //  データ抽出
    auto img = scratchImg.GetImage(0, 0, 0);

    //  WriteToSubresourceで転送する用の設定
    auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

    //  TextureBufferの作成
    ID3D12Resource* texbuff = nullptr;
    result = m_dev->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,    //  特に指定なし
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,     //  ResourceはPixelShaderで使用
        nullptr,
        IID_PPV_ARGS(&texbuff)
    );
    if (FAILED(result)) {
        return nullptr;
    }

    //  TextureのデータをSubresourceにコピー
    result = texbuff->WriteToSubresource(0,
        nullptr,        //  全領域へコピー
        img->pixels,    //  元データアドレス
        img->rowPitch,    //  1ラインサイズ
        img->slicePitch    //  全サイズ
    );
    if (FAILED(result)) {
        return nullptr;
    }

    //  作成したTextureBufferを保存
    m_textureTable[_texpath] = texbuff;

    return texbuff;
}
