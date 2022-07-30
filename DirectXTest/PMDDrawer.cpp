#include "PMDDrawer.h"

#include "MaterialData.h"
#include "PMDDrawData.h"
#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "Device.h"

PMDDrawer::PMDDrawer()
    :m_device(Singleton<Device>::GetInstance())
{
    //  処理なし
}

PMDDrawer::~PMDDrawer()
{
    //  処理なし
}

//  描画
void PMDDrawer::Draw(const PMDDrawData& _data, const ComPtr<ID3D12DescriptorHeap> _transformHeap)
{
    //  頂点バッファービューや頂点情報を設定
    m_device.dx12->GetCommandList()->IASetVertexBuffers(0, 1, &_data.vbView);
    m_device.dx12->GetCommandList()->IASetIndexBuffer(&_data.ibView);

    //  座標変換用の情報を設定
    ID3D12DescriptorHeap* transheaps[] = { _transformHeap.Get() };
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(1, transheaps);
    m_device.dx12->GetCommandList()->SetGraphicsRootDescriptorTable(1, _transformHeap->GetGPUDescriptorHandleForHeapStart());

    //  マテリアルの情報を設定
    ID3D12DescriptorHeap* mdh[] = { _data.materialHeap.Get() };
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(1, mdh);
    auto materialH = _data.materialHeap->GetGPUDescriptorHandleForHeapStart();
    unsigned int idxOffset = 0;

    //  ディスクリプタヒープのサイズの追加量
    auto cbvsrvIncSize = m_device.dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

    //  マテリアルの数だけ繰り返す
    for (auto& itr : _data.materials) {
        //  ディスクリプタヒープとルートパラメータを関連付ける
        m_device.dx12->GetCommandList()->SetGraphicsRootDescriptorTable(2, materialH);
        //  頂点情報をもとにモデルを描画
        m_device.dx12->GetCommandList()->DrawIndexedInstanced(itr.indicesNum, 1, idxOffset, 0, 0);
        //  マテリアルヒープの指定先をディスクリプタヒープのサイズだけずらす
        materialH.ptr += cbvsrvIncSize;
        //  描画した頂点数だけ頂点描画用のオフセットを加算
        idxOffset += itr.indicesNum;
    }
}