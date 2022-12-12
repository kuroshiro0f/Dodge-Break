#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <memory>

using namespace Microsoft::WRL;

//  PMD用のRendererの設定を行うクラス
class PMDRenderer
{
    friend class PMDLoader;
public:
    PMDRenderer(class Dx12Wrapper& _dx12);
    ~PMDRenderer();

    //  初期化
    void Init();

    //  ゲッター
    ID3D12PipelineState* GetPipelineState()const { return m_pipelineState.Get(); }            //  PipelineState
    ID3D12RootSignature* GetRootSignature()const { return m_rootSignature.Get(); }       //  RootSignature

private:
    Dx12Wrapper& m_dx12;
    template<typename T>
    using ComPtr = ComPtr<T>;

    //  PMD用GraphicsPipelineState
    ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;
    //  PMD用RootSignature
    ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

    //  PMD用共通Texture
    ComPtr<ID3D12Resource> m_whiteTex = nullptr;    //  白
    ComPtr<ID3D12Resource> m_blackTex = nullptr;    //  黒
    ComPtr<ID3D12Resource> m_gradTex = nullptr;     //  白から黒へのグラデーション

    //  標準Textureの生成
    //  NOTE: Textureを作成する際の共通処理
    ID3D12Resource* CreateDefaultTexture(const size_t _width, const size_t _height);
    //  白Textureの生成
    //  NOTE: TextureがないPixelで色のブレンドに乗算処理を行う場合、
    //        色を変えないために白Textureを指定する
    ID3D12Resource* CreateWhiteTexture();
    //  黒Textureの生成
    //  NOTE: TextureがないPixelで色のブレンドに加算処理を行う場合、
    //        色を変えないために黒Textureを指定する
    ID3D12Resource* CreateBlackTexture();
    //  白から黒へのグラデーションTextureの生成
    //  NOTE: ToonShadingを行わないマテリアルにデフォルトのToonTextureとして適用
    ID3D12Resource* CreateGrayGradationTexture();

    //  PMD用PipelineStateの生成
    HRESULT CreateGraphicsPipelineForPMD();
    //  PMD用RootSignatureの生成
    HRESULT CreateRootSignature();

    //  Shaderのコンパイルが成功しているか確認
    bool CheckShaderCompileResult(HRESULT _result, ID3DBlob* _error = nullptr);
};