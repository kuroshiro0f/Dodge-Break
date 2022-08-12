#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <map>
#include <unordered_map>
#include <DirectXTex.h>
#include <wrl.h>
#include <functional>
#include <iostream>

#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

//  DirectX12関連の設定を行うクラス
//  NOTE: DirectX12におけるGPUに渡す情報の扱い方として、
//        ・情報自体を持つBuffer
//        ・Bufferの扱い方を定義するView
//        があり、さらにViewはSamplerというものと合わせてDiscriptorというものとして扱います。
//        そしてDiscriptorを定義するために、Discriptorの情報を書き込むための領域の確保や扱い方を定義するためのDiscriptorHeapがあります。
//        そのため、GPUに情報を渡すための基本的な流れとしては、
//        Bufferの作成→DiscriptorHeapの作成→Discriptor(ViewやSampler)の作成
//        となります。
class Dx12Wrapper
{
public:
    Dx12Wrapper(HWND _hwnd);
    ~Dx12Wrapper();

    //  描画の開始
    void BeginDraw();
    //  描画の終了
    void EndDraw();

    //  テクスチャパスから必要なテクスチャバッファへのポインタを返す
    //  @param texpath テクスチャファイルパス
    ComPtr<ID3D12Resource> GetTextureByPath(const char* _texpath);

    //  SpriteFont用のDescriptorHeapを返す
    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForSpriteFont();

    //  CommandQueueの処理待ち
    void WaitForCommandQueue();

    //  シーン(ビュープロジェクション)をセット
    void SetScene();

    //  カメラの座標を更新
    void UpdateCameraPos(const XMFLOAT3& _eye, const XMFLOAT3& _target, const XMFLOAT3& _up);

    //  ゲッター
    ComPtr<ID3D12Device> GetDevice()const { return m_dev; }    //  Device
    ComPtr<ID3D12GraphicsCommandList> GetCommandList()const { return m_cmdList; }    //  CommandList
    ComPtr<IDXGISwapChain4> GetSwapchain()const { return m_swapchain; }        //  Swapchain
    ID3D12CommandQueue* GetCmdQue()const { return m_cmdQueue.Get(); }          //  CommandQue
    D3D12_VIEWPORT GetViewPort()const { return *m_viewport.get(); };      //  ViewPort
    XMMATRIX GetViewMatrix()const { return m_mappedCameraData->view; }    //  ビュー行列
    XMMATRIX GetProjMatrix()const { return m_mappedCameraData->proj; }    //  プロジェクション行列
private:
    template<typename T>
    using ComPtr = ComPtr<T>;

    //  ウィンドウサイズ
    SIZE m_windowSize;

    //  DXGIオブジェクト作成用のインターフェースへのポインタ
    //  NOTE: Adapterの出力に使用
    ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;

    //  Swapchain
    //  NOTE: ダブルバッファリングに使用
    ComPtr<IDXGISwapChain4> m_swapchain = nullptr;

    //  Device
    //  NOTE: Direct3Dデバイスの情報を持つ
    ComPtr<ID3D12Device> m_dev = nullptr;

    //  CommandAllocator
    //  NOTE: GPUへの処理命令を溜めておくリスト
    ComPtr<ID3D12CommandAllocator> m_cmdAllocator = nullptr;
    //  CommandList
    //  NOTE: CommandAllocatorに処理命令を溜めるためのインターフェース
    ComPtr<ID3D12GraphicsCommandList> m_cmdList = nullptr;
    //  CommandQueue
    //  NOTE: CommandAllocatorに溜めた処理命令を順番に且つ一気に実行するためのもの
    ComPtr<ID3D12CommandQueue> m_cmdQueue = nullptr;

    //  DepthBuffer
    //  NOTE: 深度用Buffer
    //  NOTE: DepthStencilBufferとして設定するが、ステンシル値は使わないためDepthBufferという扱いにしています。
    ComPtr<ID3D12Resource> m_depthBuffer = nullptr;

    //  ダブルバッファリングにおいて裏画面の情報を持つBuffer
    //  NOTE: 2つの画面を入れ替えながら描画するため配列として宣言
    std::vector<ID3D12Resource*> m_backBuffers;

    //  RenderTargetView用のDiscriptorHeap
    ComPtr<ID3D12DescriptorHeap> m_rtvHeaps = nullptr;
    //  DepthStencilView用のDiscriptorHeap
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;

    //  ViewPort
    //  NOTE: 描画する範囲を決めるためのもの
    std::unique_ptr<D3D12_VIEWPORT> m_viewport;
    //  ScissorRect
    //  NOTE: 画面に表示する範囲を決めるためのもの
    std::unique_ptr<D3D12_RECT> m_scissorrect;

    //  シェーダー側に渡すためのカメラの行列データ
    struct CameraData
    {
        XMMATRIX view;        //  ビュー行列
        XMMATRIX proj;        //  プロジェクション行列
        XMFLOAT3 eye;         //  視点座標
    };
    CameraData* m_mappedCameraData;
    //  カメラのBuffer
    ComPtr<ID3D12Resource> m_cameraConstBuff = nullptr;
    //  カメラ用のDiscriptorHeap
    ComPtr<ID3D12DescriptorHeap> m_cameraDescHeap = nullptr;
    //  カメラの座標
    XMFLOAT3 m_cameraPos = { 0,0,0 };

    //  Fence
    //  NOTE: GPUの処理が終了したか監視する
    ComPtr<ID3D12Fence> m_fence = nullptr;
    UINT64 m_fenceVal = 0;

    //  RenderTargetの作成
    HRESULT CreateRenderTargets();
    //  DepthStencilViewの作成
    HRESULT CreateDepthStencilView();
    //  SwapChainの作成
    //  NOTE: ダブルバッファリングを使用するために、
    //        表示する画面(表画面)と描画する画面(裏画面)を定義して切り替えるための仕組み
    HRESULT CreateSwapChain(const HWND& _hwnd);
    //  DXGIDevice設定
    HRESULT InitializeDXGIDevice();
    //  CommandList関連の作成
    HRESULT InitializeCommand();
    //  カメラの作成
    HRESULT CreateCamera();

    //  Textureファイルロード用関数へのポインタのマップ配列
    using LoadLambda_t = std::function<HRESULT(const std::wstring& _path, TexMetadata*, ScratchImage&)>;
    std::map<std::string, LoadLambda_t> m_loadLambdaTable;
    //  Textureファイル情報の配列
    std::unordered_map<std::string, ComPtr<ID3D12Resource>> m_textureTable;
    //  TextureLoaderTableの作成
    //  NOTE: Textureのファイルパスの記述子の種類によって使用する関数を分ける
    void CreateTextureLoaderTable();
    //  Texture名からTextureBuffer作成
    ID3D12Resource* CreateTextureFromFile(const char* _texpath);
};