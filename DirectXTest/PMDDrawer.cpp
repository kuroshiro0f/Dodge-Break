#include "PMDDrawer.h"

#include "MaterialData.h"
#include "PMDModelData.h"
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

//  描画命令の発行
void PMDDrawer::IssueDrawCommand(const struct PMDModelData& _data, const ComPtr<ID3D12DescriptorHeap> _transformHeap, const int _num)
{
    //  VertexBufferおよびIndexBufferをそれぞれのViewと紐づける
    m_device.dx12->GetCommandList()->IASetVertexBuffers(0, 1, &_data.vbView);
    m_device.dx12->GetCommandList()->IASetIndexBuffer(&_data.ibView);

    //  座標変換用の情報を設定
    //  座標変換用DiscriptorHeap
    ID3D12DescriptorHeap* transheaps[] = { _transformHeap.Get() };
    //  座標変換用DiscriptorHeapを設定する命令を発行
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(1, transheaps);
    //  座標変換用DescriptorをGraphicsRootSignatrueに設定する命令を発行
    m_device.dx12->GetCommandList()->SetGraphicsRootDescriptorTable(1, _transformHeap->GetGPUDescriptorHandleForHeapStart());

    //  Materialの情報を設定
    //  Material用DiscriptorHeap
    ID3D12DescriptorHeap* mdh[] = { _data.materialHeap.Get() };
    //  Material用DiscriptorHeapを設定する命令を発行
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(1, mdh);
    //  Material用DiscriptorHeapの先頭アドレスを取得
    auto materialH = _data.materialHeap->GetGPUDescriptorHandleForHeapStart();
    //  頂点描画用のOffset
    unsigned int idxOffset = 0;

    //  DiscriptorHeapの指定先をずらすための値
    auto cbvsrvIncSize = m_device.dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

    //  Materialの数だけ繰り返す
    for (auto& itr : _data.materials) {
        // DiscriptorHeapとRootParameterを関連付ける命令を発行
        m_device.dx12->GetCommandList()->SetGraphicsRootDescriptorTable(2, materialH);
        //  頂点情報と描画数をもとにモデルのインスタンシング描画の命令を発行
        m_device.dx12->GetCommandList()->DrawIndexedInstanced(itr.indicesNum, _num, idxOffset, 0, 0);
        //  Material用DiscriptorHeapの指定先を今回設定したMaterialののサイズだけずらす
        materialH.ptr += cbvsrvIncSize;
        //  描画した頂点数だけ頂点描画用のOffsetを加算
        idxOffset += itr.indicesNum;
    }
}