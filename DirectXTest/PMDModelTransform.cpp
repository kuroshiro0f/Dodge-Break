#include "PMDModelTransform.h"

#include <d3dx12.h>

#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "Device.h"

PMDModelTransform::PMDModelTransform()
    :m_device(Singleton<Device>::GetInstance())
    , m_transformMat(nullptr)
    , m_transformHeap(nullptr)
    , m_transform({})
    , m_mappedTransform(nullptr)
    , m_transformBuff(nullptr)
{
    //  処理なし
}

PMDModelTransform::~PMDModelTransform()
{
    //  処理なし
}

//  初期化
void PMDModelTransform::Init()
{
    //  座標変換用ポインタの初期化
    for (int i = 0; i < IncetancedDrawData::num; i++)
    {
        m_transform.world[i] = XMMatrixIdentity();
    }

    //  座標変換用Viewの生成
    CreateTransformView();
}

//  座標変換用ビューの生成
HRESULT PMDModelTransform::CreateTransformView()
{
    //  座標変換用Bufferのサイズを設定
    auto buffSize = sizeof(Transform);
    buffSize = (buffSize + 0xff) & ~0xff;
    //  HeapPropertiesをUploadで設定
    auto temp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    //  ResourceDescを設定
    auto temp2 = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
    //  座標変換用Bufferの作成
    auto result = m_device.dx12->GetDevice()->CreateCommittedResource(
        &temp1,
        D3D12_HEAP_FLAG_NONE,       //  特に指定なし
        &temp2,
        D3D12_RESOURCE_STATE_GENERIC_READ,  //  Uploadで設定されたHeapPropertiesに合わせる
        nullptr,
        IID_PPV_ARGS(m_transformBuff.ReleaseAndGetAddressOf())
    );
    if (FAILED(result))
    {
        assert(SUCCEEDED(result));
        return result;
    }

    //  作成した座標変換用BufferをGPUに渡して、その仮想アドレスをm_mappedTransformに格納(map)
    //  NOTE: Unmapは行わず仮想アドレスからのアクセスを常に行えるようにし、座標変換を反映させます
    result = m_transformBuff->Map(0, nullptr, (void**)&m_mappedTransform);
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }
    *m_mappedTransform = m_transform;

    //  座標変換用DiscriptorHeapの設定
    D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
    transformDescHeapDesc.NumDescriptors = 1;    //  Discriptorは1つ(ワールド座標のみ)
    transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;    //  Shaderから見えるように
    transformDescHeapDesc.NodeMask = 0;     //  操作するAdoptorは単一
    transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;    //  DiscriptorHeapの種別を設定
    //  座標変換用Heapの生成
    result = m_device.dx12->GetDevice()->CreateDescriptorHeap(&transformDescHeapDesc, IID_PPV_ARGS(m_transformHeap.ReleaseAndGetAddressOf()));
    if (FAILED(result)) {
        assert(SUCCEEDED(result));
        return result;
    }

    //  座標変換用Viewを設定
    //  NOTE: 行列を渡すため定数BufferView
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_transformBuff->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = buffSize;
    //  座標変換用Viewを生成
    m_device.dx12->GetDevice()->CreateConstantBufferView(&cbvDesc, m_transformHeap->GetCPUDescriptorHandleForHeapStart());

    return S_OK;
}
