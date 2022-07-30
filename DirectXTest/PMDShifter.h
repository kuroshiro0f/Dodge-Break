#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class PMDShifter
{
public:
    PMDShifter();
    ~PMDShifter();

    //  初期化
    void Init();

    //  座標更新
    //  TODO: 同じ関数名にせず、処理ごとに関数名を変更
    void Update(const XMFLOAT3& _pos) { m_mappedTransform->world = XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }
    void Update(const XMFLOAT3& _pos, const float _angle) { m_mappedTransform->world = XMMatrixRotationY(_angle) * XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }

    //  座標と拡大率の更新
    void UpdatePosAndScale(const XMFLOAT3& _pos, const float _scale) { m_mappedTransform->world = XMMatrixScaling(_scale,_scale,_scale) * XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }

    //  描画に必要なデータのゲッター
    ComPtr<ID3D12DescriptorHeap> GetTransformHeap()const { return m_transformHeap; };
private:
    //  座標変換用ビューの生成
    HRESULT CreateTransformView();
    //  ワールド座標変換行列
    struct Transform
    {
        //  内部に持ってるXMMATRIXメンバが16バイトアライメントであるため
        //  Transformをnewする際には16バイト境界に確保する
        void* operator new(size_t _size);
        XMMATRIX world;
    };

    //  座標変換関連
    ComPtr<ID3D12Resource> m_transformMat;          //  行列
    ComPtr<ID3D12DescriptorHeap> m_transformHeap;   //  ヒープ
    Transform m_transform;                          //  ワールド座標変換行列
    Transform* m_mappedTransform;                   //  ワールド座標変換行列のポインタ
    ComPtr<ID3D12Resource> m_transformBuff;         //  バッファー

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
};