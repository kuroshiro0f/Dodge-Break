#include "PMDRenderer.h"

#include <d3dx12.h>
#include <d3dcompiler.h>
#include <cassert>
#include <string>
#include <algorithm>

#include "Dx12Wrapper.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace std;

PMDRenderer::PMDRenderer(Dx12Wrapper& _dx12)
    :m_dx12(_dx12)
{
    //  処理なし
}

PMDRenderer::~PMDRenderer()
{
    //  処理なし
}

//  初期化
void PMDRenderer::Init()
{
    //  RootSignature生成
    CreateRootSignature();
    //  GraphicsPipelineState生成
    CreateGraphicsPipelineForPMD();
    //  白Texture生成
    m_whiteTex = CreateWhiteTexture();
    //  黒Texture生成
    m_blackTex = CreateBlackTexture();
    //  白から黒へのグラデーションTexture生成
    m_gradTex = CreateGrayGradationTexture();
}

//  標準Textureの生成
//  NOTE: Textureを作成する際の共通処理
ID3D12Resource* PMDRenderer::CreateDefaultTexture(const size_t _width, const size_t _height)
{
    //  2DTextureとして設定
    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, _width, _height);
    //  キャッシュメモリを介してメインメモリに書き込む方式(WriteBack)を設定
    auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    ID3D12Resource* buff = nullptr;
    //  標準Textureの生成
    auto result = m_dx12.GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,    //  特に指定なし
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,     //  PixelShaderが扱うResourceとして設定
        nullptr,
        IID_PPV_ARGS(&buff)
    );
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return nullptr;
    }
    return buff;
}

//  白Textureの生成
//  NOTE: TextureがないPixelで色のブレンドに乗算処理を行う場合、
//        色を変えないために白Textureを指定する
ID3D12Resource* PMDRenderer::CreateWhiteTexture()
{
    //  標準テクスチャを生成
    ID3D12Resource* whiteBuff = CreateDefaultTexture(4, 4);

    //  全ての値を255に
    vector<unsigned char> data(4 * 4 * 4);
    fill(data.begin(), data.end(), 0xff);

    //  TextureのデータをSubresourceにコピー
    auto result = whiteBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    assert(SUCCEEDED(result));
    return whiteBuff;
}

//  黒Textureの生成
//  NOTE: TextureがないPixelで色のブレンドに加算処理を行う場合、
//        色を変えないために黒Textureを指定する
ID3D12Resource* PMDRenderer::CreateBlackTexture()
{
    //  標準テクスチャを生成
    ID3D12Resource* blackBuff = CreateDefaultTexture(4, 4);

    //  全ての値を0に
    vector<unsigned char> data(4 * 4 * 4);
    fill(data.begin(), data.end(), 0x00);

    //  TextureのデータをSubresourceにコピー
    auto result = blackBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    assert(SUCCEEDED(result));
    return blackBuff;
}

//  白から黒へのグラデーションTextureの生成
//  NOTE: ToonShadingを行わないマテリアルにデフォルトのToonTextureとして適用
ID3D12Resource* PMDRenderer::CreateGrayGradationTexture()
{
    //  標準テクスチャを生成
    ID3D12Resource* gradBuff = CreateDefaultTexture(4, 256);

    //  上が白くて下が黒いTextureデータを作成
    vector<unsigned int> data(4 * 256);
    auto it = data.begin();
    unsigned int c = 0xff;
    for (; it != data.end(); it += 4) {
        auto col = (c << 0xff) | (c << 16) | (c << 8) | c;
        std::fill(it, it + 4, col);
        --c;
    }

    //  TextureのデータをSubresourceにコピー
    auto result = gradBuff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int), sizeof(unsigned int) * data.size());
    assert(SUCCEEDED(result));
    return gradBuff;
}

//  PMD用のGraphicsPipelineStateの生成
HRESULT PMDRenderer::CreateGraphicsPipelineForPMD()
{
    //  VertexShader(頂点シェーダー)
    ComPtr<ID3DBlob> vsBlob = nullptr;
    //  PixelShader(ピクセルシェーダー)
    ComPtr<ID3DBlob> psBlob = nullptr;
    //  エラーメッセージ取得用
    ComPtr<ID3DBlob> errorBlob = nullptr;

    //  バインドされていない記述子テーブルを有効にする
    UINT compileFlags = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

    //  VertexShaderをコンパイル
    auto result = D3DCompileFromFile
        (L"VertexShader.hlsl",  //  ファイル名
        nullptr,
        nullptr,
        "BasicVS",      //  Shaderのメイン関数名
        "vs_5_1",       //  Shaderのバージョン
        compileFlags,
        0,
        &vsBlob, 
        &errorBlob);
    //  VertexShaderのコンパイルに成功したか確認
    if (!CheckShaderCompileResult(result, errorBlob.Get())) {
        assert(0);
        return result;
    }
    //  PixelShaderをコンパイル
    result = D3DCompileFromFile
        (L"PixelShader.hlsl",
        nullptr, 
        nullptr,
        "BasicPS", 
        "ps_5_1",
        compileFlags,
        0,
        &psBlob,
        &errorBlob);
    //  PixelShaderのコンパイルに成功したか確認
    if (!CheckShaderCompileResult(result, errorBlob.Get())) {
        assert(0);
        return result;
    }

    //  頂点Layoutの定義
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
    };

    //  GraphicsPipelineStateの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

    //  RootSignatureのセット
    gpipeline.pRootSignature = m_rootSignature.Get();
    //  Shaderのセット
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    //  Sample設定
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;    //  中身は0xffffffff
    //  BlendState設定
    gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);   //  デフォルトの設定

    //  RasterizerState設定
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); //  デフォルトの設定
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;    //  カリングしない

    //  DepthStencilBuffer設定
    gpipeline.DepthStencilState.DepthEnable = true;    //  深度テストを有効にする
    gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;   //  全て書き込み
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;        //  小さい方を採用
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;        //  32ビットの浮動小数点形式
    gpipeline.DepthStencilState.StencilEnable = false;  //  StencilBufferの使用はしない
    gpipeline.InputLayout.pInputElementDescs = inputLayout;       //  頂点Layout先頭アドレス
    gpipeline.InputLayout.NumElements = _countof(inputLayout);    //  頂点Layout配列数

    //  頂点のつなげ方を設定
    gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;     //  Strip(頂点を結び連続して存在する三角形)同士はカットしない(不連続性を持たせない)。
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;    //  三角形で構成

    //  RenderTargetの設定
    gpipeline.NumRenderTargets = 1;    //  RenderTargetは1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;    //  0～1に正規化されたRGBA

    //  Samplingの設定
    gpipeline.SampleDesc.Count = 1;         //  1PixelあたりのMultiSampling数は1
    gpipeline.SampleDesc.Quality = 0;       //  Qualityを最低にし、パフォーマンスを優先する

    //  GraphicsPipelineStateの生成
    result = m_dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(m_pipelineState.GetAddressOf()));
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
    }
    return result;
}

//  RootSignatureの生成
HRESULT PMDRenderer::CreateRootSignature()
{
    //  DiscriptorRangeの設定
    //  NOTE: Shaderのどのスロット番号に値を設定するか決定する
    CD3DX12_DESCRIPTOR_RANGE  discriptorRanges[4] = {};    //  Textureと定数3つ
    discriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);    //  定数[b0](ビュープロジェクション行列用)
    discriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);    //  定数[b1](ワールド行列用)
    discriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);    //  定数[b0](マテリアル用)
    discriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);    //  Texture４つ(基本とsphとspaとトゥーン)

    //  RootParameter
    //  NOTE: 設定したDiscriptorRangeをどのShaderに紐づけるか設定
    CD3DX12_ROOT_PARAMETER rootParams[3] = {};
    rootParams[0].InitAsDescriptorTable(1, &discriptorRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);  //  ビュープロジェクション行列情報の紐づけ先を頂点シェーダーに設定
    rootParams[1].InitAsDescriptorTable(1, &discriptorRanges[1], D3D12_SHADER_VISIBILITY_VERTEX);  //  ワールド座標情報の紐づけ先を頂点シェーダーに設定
    rootParams[2].InitAsDescriptorTable(2, &discriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);   //  MaterialとTextureデータの紐づけ先をピクセルシェーダーに設定

    //  Sampler
    //  NOTE: uv値によってTextureデータからどのように色を取り出すか決定
    CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
    samplerDescs[0].Init(0);        //  PMDモデルのTextureデータ用Sampler
    samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);  //  ToonShading用Sampler

    //  RootSignatureの設定
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init(_countof(rootParams), rootParams, _countof(samplerDescs), samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    //  RootSignature
    ComPtr<ID3DBlob> rootSigBlob = nullptr;
    //  エラーメッセージの取得用
    ComPtr<ID3DBlob> errorBlob = nullptr;
    //  RootSignatureの生成時に渡す設定をシリアル化
    auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(result)) 
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  RootSignatureの生成
    result = m_dx12.GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()));
    if (FAILED(result))
    {
        assert(SUCCEEDED(result));
        return result;
    }

    return result;
}

//  Shaderのコンパイルが成功したか確認
bool PMDRenderer::CheckShaderCompileResult(HRESULT _result, ID3DBlob* _error)
{
    if (FAILED(_result)) {
        if (_result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            ::OutputDebugStringA("ファイルが見当たりません");
        }
        else {
            std::string errstr;
            errstr.resize(_error->GetBufferSize());
            std::copy_n((char*)_error->GetBufferPointer(), _error->GetBufferSize(), errstr.begin());
            errstr += "\n";
            OutputDebugStringA(errstr.c_str());
        }
        return false;
    }
    else {
        return true;
    }
}
