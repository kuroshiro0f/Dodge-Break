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

    //  描画
    void Draw(const struct PMDDrawData& _data,const ComPtr<ID3D12DescriptorHeap> _transformHeap);
private:
    //  シングルトンクラスへの参照
    class Device& m_device; //  デバイスの管理
};