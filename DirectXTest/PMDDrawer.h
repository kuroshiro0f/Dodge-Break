#pragma once
#include <d3d12.h>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

//  PMDモデルの描画を行うクラス
class PMDDrawer
{
public:
    PMDDrawer();
    ~PMDDrawer();

    //  描画命令の発行
    void IssueDrawCommand(const struct PMDModelData& _data,const ComPtr<ID3D12DescriptorHeap> _transformHeap, const int _num);
private:
    //  シングルトンクラスへの参照
    class Device& m_device; //  Device
};