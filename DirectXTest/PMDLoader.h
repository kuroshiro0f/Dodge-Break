#pragma once
#include <d3d12.h>
#include <vector>
#include <wrl/client.h>

#include "MaterialData.h"

using namespace Microsoft::WRL;

//  PMDモデルのロードを行うクラス
class PMDLoader
{
    friend class PMDRenderer;
public:
    PMDLoader();
    ~PMDLoader();

    //  モデルのロード
    void LoadModel(const char* _filepath);

    //  描画に必要なデータのゲッター
    D3D12_VERTEX_BUFFER_VIEW GetVbView()const { return m_vbView; }
    D3D12_INDEX_BUFFER_VIEW GetIbView()const { return m_ibView; }
    std::vector<struct Material> GetMaterials()const;
    ComPtr<ID3D12DescriptorHeap> GetMaterialHeap()const { return m_materialHeap; };
private:
    //  読み込んだマテリアルをもとにマテリアルバッファを作成
    HRESULT CreateMaterialData();

    //  マテリアル＆テクスチャのビューを生成
    HRESULT CreateMaterialAndTextureView();

    //  PMDファイルのロード
    HRESULT LoadPMDFile(const char* _path);

    //  頂点関連
    ComPtr<ID3D12Resource> m_vb;
    ComPtr<ID3D12Resource> m_ib;
    D3D12_VERTEX_BUFFER_VIEW m_vbView;
    D3D12_INDEX_BUFFER_VIEW m_ibView;

    // マテリアル関連
    std::vector<struct Material> m_materials;                 //  マテリアル
    ComPtr<ID3D12Resource> m_materialBuff;                    //  バッファー
    std::vector<ComPtr<ID3D12Resource>> m_textureResources;   //  テクスチャ
    std::vector<ComPtr<ID3D12Resource>> m_sphResources;       //  sph
    std::vector<ComPtr<ID3D12Resource>> m_spaResources;       //  spa
    std::vector<ComPtr<ID3D12Resource>> m_toonResources;      //  toon
    ComPtr<ID3D12DescriptorHeap> m_materialHeap;              //  ヒープ

    //  シングルトンクラスへの参照
    class Device& m_device; //  デバイスの管理
};