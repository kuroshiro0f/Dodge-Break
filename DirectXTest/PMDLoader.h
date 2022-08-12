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
    D3D12_VERTEX_BUFFER_VIEW GetVbView()const { return m_vbView; }  //  VertexBufferView
    D3D12_INDEX_BUFFER_VIEW GetIbView()const { return m_ibView; }   //  IndexBufferView
    std::vector<struct Material> GetMaterials()const;   //  Material情報の配列
    ComPtr<ID3D12DescriptorHeap> GetMaterialHeap()const { return m_materialHeap; }; //  Material用DiscriptorHeap
private:
    //  読み込んだMaterialをもとにMaterialBufferを作成
    HRESULT CreateMaterialData();

    //  MaterialとTextureのViewを生成
    HRESULT CreateMaterialAndTextureView();

    //  PMDファイルのロード
    HRESULT LoadPMDFile(const char* _path);

    //  頂点関連
    ComPtr<ID3D12Resource> m_vb;        //  VertexBuffer
    ComPtr<ID3D12Resource> m_ib;        //  IndexBufferView
    D3D12_VERTEX_BUFFER_VIEW m_vbView;  //  VertexBufferView
    D3D12_INDEX_BUFFER_VIEW m_ibView;   //  IndexBufferView

    //  Material関連
    std::vector<struct Material> m_materials;                 //  Material情報の配列
    ComPtr<ID3D12Resource> m_materialBuff;                    //  MaterialBuffer
    std::vector<ComPtr<ID3D12Resource>> m_textureResources;   //  TextureBuffer
    std::vector<ComPtr<ID3D12Resource>> m_sphResources;       //  sphBuffer
    std::vector<ComPtr<ID3D12Resource>> m_spaResources;       //  spaBuffer
    std::vector<ComPtr<ID3D12Resource>> m_toonResources;      //  toonBuffer
    ComPtr<ID3D12DescriptorHeap> m_materialHeap;              //  Material用DiscriptorHeap

    //  シングルトンクラスへの参照
    class Device& m_device; //  Device
};