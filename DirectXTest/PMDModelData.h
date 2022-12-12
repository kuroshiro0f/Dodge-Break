#pragma once
#include <d3d12.h>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

//  PMDの描画の際に必要なデータ
struct PMDModelData
{
    //  頂点バッファービュー
    D3D12_VERTEX_BUFFER_VIEW vbView;
    D3D12_INDEX_BUFFER_VIEW ibView;
    //  マテリアル
    std::vector<struct Material> materials;
    //  マテリアルヒープ
    ComPtr<ID3D12DescriptorHeap> materialHeap;
};