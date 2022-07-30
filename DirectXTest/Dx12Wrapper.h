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

    //  ゲッター
    ComPtr<ID3D12Device> GetDevice()const { return m_dev; }    //  デバイス
    ComPtr<ID3D12GraphicsCommandList> GetCommandList()const { return m_cmdList; }    //  CommandList
    ComPtr<IDXGISwapChain4> GetSwapchain()const { return m_swapchain; }        //  Swapchain
    ID3D12CommandQueue* GetCmdQue()const { return m_cmdQueue.Get(); }          //  CommandQue

    //  ViewPortを返す
    D3D12_VIEWPORT GetViewPort()const;

    //  SpriteFont用のDescriptorHeapを返す
    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForSpriteFont();

    //  CommandQueueの処理待ち
    void WaitForCommandQueue();

    //  シーン(ビュープロジェクション)をセット
    void SetScene();

    //  カメラの座標を更新
    void UpdateCameraPos(const XMFLOAT3& _eye, const XMFLOAT3& _target, const XMFLOAT3& _up);

    //  ゲッター
    XMMATRIX GetViewMatrix()const { return m_mappedSceneData->view; }    //  ビュー行列
    XMMATRIX GetProjMatrix()const { return m_mappedSceneData->proj; }    //  プロジェクション行列

private:
    SIZE m_winSize;
    template<typename T>
    using ComPtr = ComPtr<T>;

    //  DXGIまわり
    ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;    //  DXGIインターフェイス
    ComPtr<IDXGISwapChain4> m_swapchain = nullptr;    //  スワップチェイン

    //  DirectX12まわり
    ComPtr<ID3D12Device> m_dev = nullptr;        //  デバイス
    ComPtr<ID3D12CommandAllocator> m_cmdAllocator = nullptr;    //  コマンドアロケータ
    ComPtr<ID3D12GraphicsCommandList> m_cmdList = nullptr;      //  コマンドリスト
    ComPtr<ID3D12CommandQueue> m_cmdQueue = nullptr;            //  コマンドキュー

    //  表示に関わるバッファまわり
    ComPtr<ID3D12Resource> m_depthBuffer = nullptr;    //  深度バッファ
    std::vector<ID3D12Resource*> m_backBuffers;    //  バックバッファ
    ComPtr<ID3D12DescriptorHeap> m_rtvHeaps = nullptr;    //  レンダーターゲット用ディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;     //  深度バッファビュー用ディスクリプタヒープ
    std::unique_ptr<D3D12_VIEWPORT> m_viewport;    //  ビューポート
    std::unique_ptr<D3D12_RECT> m_scissorrect;     //  シザー矩形

    //  シーンを構成するバッファまわり
    ComPtr<ID3D12Resource> m_sceneConstBuff = nullptr;
    
    //  シェーダー側に渡すための基本的な行列データ
    struct SceneData
    {
        XMMATRIX view;        //  ビュー行列
        XMMATRIX proj;        //  プロジェクション行列
        XMFLOAT3 eye;         //  視点座標
    };

    //  シェーダに渡す行列データ
    SceneData* m_mappedSceneData;
    ComPtr<ID3D12DescriptorHeap> m_sceneDescHeap = nullptr;        //  ヒープ

    //  Fence
    ComPtr<ID3D12Fence> m_fence = nullptr;
    UINT64 m_fenceVal = 0;

    //  FinalRenderTargetの生成
    HRESULT CreateFinalRenderTargets();

    //  DepthStencilViewの生成
    HRESULT CreateDepthStencilView();

    //  SwapChainの生成
    HRESULT CreateSwapChain(const HWND& _hwnd);

    //  DXGIまわり初期化
    HRESULT InitializeDXGIDevice();

    //  Commandまわり初期化
    HRESULT InitializeCommand();

    //  ビュープロジェクション用ビューの生成
    HRESULT CreateSceneView();

    //  ロード用テーブル
    using LoadLambda_t = std::function<HRESULT(const std::wstring& _path, TexMetadata*, ScratchImage&)>;
    std::map<std::string, LoadLambda_t> m_loadLambdaTable;
    //  テクスチャテーブル
    std::unordered_map<std::string, ComPtr<ID3D12Resource>> m_textureTable;
    //  TextureLoaderTableの作成
    void CreateTextureLoaderTable();
    //  テクスチャ名からテクスチャバッファ作成、中身をコピー
    ID3D12Resource* CreateTextureFromFile(const char* _texpath);

    ComPtr<ID3D12DescriptorHeap> CreateOneDescriptorHeapSRV();

    XMFLOAT3 m_cameraPos = { 0,0,0 };
};