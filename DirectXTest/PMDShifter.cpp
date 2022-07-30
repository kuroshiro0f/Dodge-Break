#include "PMDShifter.h"

#include <d3dx12.h>

#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "Device.h"

PMDShifter::PMDShifter()
    :m_device(Singleton<Device>::GetInstance())
    , m_transformMat(nullptr)
    , m_transformHeap(nullptr)
    , m_transform({})
    , m_mappedTransform(nullptr)
    , m_transformBuff(nullptr)
{
}

PMDShifter::~PMDShifter()
{
    //  処理なし
}

//  初期化
void PMDShifter::Init()
{
    //  座標変換用ポインタの初期化
    m_transform.world = XMMatrixIdentity();
    //  座標変換用ビューの生成
    CreateTransformView();
}

//  内部に持ってるXMMATRIXメンバが16バイトアライメントであるため
//  Transformをnewする際には16バイト境界に確保する
void* PMDShifter::Transform::operator new(size_t _size)
{
    return _aligned_malloc(_size, 16);
}

//  座標変換用ビューの生成
HRESULT PMDShifter::CreateTransformView()
{
    //  座標変換用バッファの作成と情報の転送
    auto buffSize = sizeof(Transform);
    buffSize = (buffSize + 0xff) & ~0xff;
    auto temp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto temp2 = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
    auto result = m_device.dx12->GetDevice()->CreateCommittedResource(
        &temp1,
        D3D12_HEAP_FLAG_NONE,
        &temp2,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_transformBuff.ReleaseAndGetAddressOf())
    );
    if (FAILED(result))
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  作成した情報をコピー
    result = m_transformBuff->Map(0, nullptr, (void**)&m_mappedTransform);
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }
    *m_mappedTransform = m_transform;

    //  座標変換用ビューの作成
    D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
    transformDescHeapDesc.NumDescriptors = 1;    //  ワールドひとつ
    transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    transformDescHeapDesc.NodeMask = 0;
    transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;    //  デスクリプタヒープ種別
    result = m_device.dx12->GetDevice()->CreateDescriptorHeap(&transformDescHeapDesc, IID_PPV_ARGS(m_transformHeap.ReleaseAndGetAddressOf()));    //  生成
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }

    //  定数バッファビューの作成
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_transformBuff->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = buffSize;
    m_device.dx12->GetDevice()->CreateConstantBufferView(&cbvDesc, m_transformHeap->GetCPUDescriptorHandleForHeapStart());

    return S_OK;
}
