#include "PMDRenderer.h"

#include <d3dx12.h>
#include <d3dcompiler.h>
#include <cassert>
#include <string>
#include <algorithm>

#include "Dx12Wrapper.h"

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
    //  ルートシグネチャ生成
    CreateRootSignature();
    //  グラフィックスパイプライン生成
    CreateGraphicsPipelineForPMD();
    //  白テクスチャ生成
    m_whiteTex = CreateWhiteTexture();
    //  黒テクスチャ生成
    m_blackTex = CreateBlackTexture();
    //  グレーテクスチャ生成
    m_gradTex = CreateGrayGradationTexture();
}

//  標準のテクスチャの生成
ID3D12Resource* PMDRenderer::CreateDefaultTexture(const size_t _width, const size_t _height)
{
    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, _width, _height);
    auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    ID3D12Resource* buff = nullptr;
    auto result = m_dx12.GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,    //  特に指定なし
        &resDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&buff)
    );
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return nullptr;
    }
    return buff;
}

//  白テクスチャの生成
ID3D12Resource* PMDRenderer::CreateWhiteTexture()
{
    ID3D12Resource* whiteBuff = CreateDefaultTexture(4, 4);

    vector<unsigned char> data(4 * 4 * 4);
    fill(data.begin(), data.end(), 0xff);

    auto result = whiteBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    assert(SUCCEEDED(result));
    return whiteBuff;
}

//  黒テクスチャの生成
ID3D12Resource* PMDRenderer::CreateBlackTexture()
{
    ID3D12Resource* blackBuff = CreateDefaultTexture(4, 4);
    vector<unsigned char> data(4 * 4 * 4);
    fill(data.begin(), data.end(), 0x00);

    auto result = blackBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    assert(SUCCEEDED(result));
    return blackBuff;
}

//  グレースケールグラデーションテクスチャの生成
ID3D12Resource* PMDRenderer::CreateGrayGradationTexture()
{
    ID3D12Resource* gradBuff = CreateDefaultTexture(4, 256);
    //  上が白くて下が黒いテクスチャデータを作成
    vector<unsigned int> data(4 * 256);
    auto it = data.begin();
    unsigned int c = 0xff;
    for (; it != data.end(); it += 4) {
        auto col = (c << 0xff) | (c << 16) | (c << 8) | c;
        std::fill(it, it + 4, col);
        --c;
    }

    auto result = gradBuff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int), sizeof(unsigned int) * data.size());
    assert(SUCCEEDED(result));
    return gradBuff;
}

//  PMD用のパイプラインの生成
HRESULT PMDRenderer::CreateGraphicsPipelineForPMD()
{
    //  シェーダーの設定
    ComPtr<ID3DBlob> vsBlob = nullptr;
    ComPtr<ID3DBlob> psBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    //  シェーダーをコンパイル
    auto result = D3DCompileFromFile(L"BasicShader.hlsl",
        nullptr, nullptr,
        "BasicVS", "vs_5_0",
        0,
        0, &vsBlob, &errorBlob);
    if (!CheckShaderCompileResult(result, errorBlob.Get())) {
        assert(0);
        return result;
    }
    result = D3DCompileFromFile(L"BasicShader.hlsl",
        nullptr, nullptr,
        "BasicPS", "ps_5_0",
        0,
        0, &psBlob, &errorBlob);
    if (!CheckShaderCompileResult(result, errorBlob.Get())) {
        assert(0);
        return result;
    }
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
    };

    //  グラフィックスパイプラインステートの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
    gpipeline.pRootSignature = m_rootSignature.Get();
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;    //  中身は0xffffffff

    gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;    //  カリングしない

    gpipeline.DepthStencilState.DepthEnable = true;    //  深度バッファを使う
    gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;    //  全て書き込み
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;        //  小さい方を採用
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    gpipeline.DepthStencilState.StencilEnable = false;

    gpipeline.InputLayout.pInputElementDescs = inputLayout;       //  レイアウト先頭アドレス
    gpipeline.InputLayout.NumElements = _countof(inputLayout);    //  レイアウト配列数

    gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;     //  ストリップ時のカットなし
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;    //  三角形で構成

    gpipeline.NumRenderTargets = 1;    //  今は１つのみ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;    //  0～1に正規化されたRGBA

    gpipeline.SampleDesc.Count = 1;        //  サンプリングは1ピクセルにつき１
    gpipeline.SampleDesc.Quality = 0;      //  クオリティは最低

    //  グラフィックスパイプラインステートの生成
    result = m_dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(m_pipeline.GetAddressOf()));
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
    }
    return result;
}

//  ルートシグネチャの生成
HRESULT PMDRenderer::CreateRootSignature()
{
    //  レンジ
    CD3DX12_DESCRIPTOR_RANGE  descTblRanges[4] = {};    //  テクスチャと定数の２つ
    descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);    //  定数[b0](ビュープロジェクション用)
    descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);    //  定数[b1](ワールド、ボーン用)
    descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);    //  定数[b2](マテリアル用)
    descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);    //  テクスチャ４つ(基本とsphとspaとトゥーン)

    //  ルートパラメータ
    CD3DX12_ROOT_PARAMETER rootParams[3] = {};
    rootParams[0].InitAsDescriptorTable(1, &descTblRanges[0]);    //  ビュープロジェクション変換
    rootParams[1].InitAsDescriptorTable(1, &descTblRanges[1]);    //  ワールド・ボーン変換
    rootParams[2].InitAsDescriptorTable(2, &descTblRanges[2]);    //  マテリアル周り

    //  サンプラー
    CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
    samplerDescs[0].Init(0);
    samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    //  ルートシグネチャの設定
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init(3, rootParams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    //  ルートシグネチャの生成
    ComPtr<ID3DBlob> rootSigBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(result)) 
    {
        assert(SUCCEEDED(result));
        return result;
    }

    try
    {
        result = m_dx12.GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()));
        if (FAILED(result))
        {
            assert(SUCCEEDED(result));
            return result;
        }
        if (m_dx12.GetDevice() == nullptr)
        {
            throw 1;
        }
    }
    catch (int fError)
    {
        if (fError == 1)
        {
            result = E_FAIL;
        }
    }

    return result;
}

//  シェーダーに渡せたかのチェック
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
