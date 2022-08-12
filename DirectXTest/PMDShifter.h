#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "IncetancedDrawData.h"

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
    void Update(const XMFLOAT3& _pos,int _num) { m_mappedTransform->world[_num] = XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }
    void Update(const XMFLOAT3& _pos, const float _angle, int _num) { m_mappedTransform->world[_num] = XMMatrixRotationY(_angle) * XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }

    //  座標と拡大率の更新
    void UpdatePosAndScale(const XMFLOAT3& _pos, const float _scale, int _num) { m_mappedTransform->world[_num] = XMMatrixScaling(_scale,_scale,_scale) * XMMatrixTranslation(_pos.x, _pos.y, _pos.z); }

    //  描画に必要なデータのゲッター
    ComPtr<ID3D12DescriptorHeap> GetTransformHeap()const { return m_transformHeap; };
private:
    //  座標変換用ビューの生成
    HRESULT CreateTransformView();

    //  ワールド座標変換行列
    //  NOTE: 最大描画数を要素数とする静的配列として宣言し、
    //        配列ごとシェーダーに渡してインスタンシング描画を行っています。
    //        そのため、最大描画数に満たない描画数の場合は、その差だけメモリの無駄がありますが、現状はこのシステムのまま進めます。
    struct Transform
    {
        XMMATRIX world[IncetancedDrawData::num];
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