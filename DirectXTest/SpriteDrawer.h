#pragma once
#include <d3d12.h>
#include <DirectXHelpers.h>
#include <DirectXMath.h>
#include <DescriptorHeap.h>
#include <SpriteBatch.h>
#include <vector>
#include <iostream>

using namespace Microsoft::WRL;
using namespace DirectX;

enum SpriteType;
//  画像のロードや描画を行うクラス
//  シングルトンで扱う
class SpriteDrawer {
public:
    SpriteDrawer();
    ~SpriteDrawer();

    //  初期化
    void Init();

    //  描画
    void Draw(const SpriteType _handle, const XMFLOAT2& _pos, const float _magnification = 1.0f, const XMVECTOR& _color = Colors::White);
    //  画像の描画範囲を指定する場合
    void Draw(const SpriteType _handle, const XMFLOAT2& _pos
        , const float _topMagnification, const float _leftMagnification, const float _bottomMagnification, const float _rightMagnification
        , const float _magnification = 1.0f, const XMVECTOR& _color = Colors::White);
private:
    //  ロード
    int Load();

    //  テクスチャのロード関連
    std::unique_ptr<DescriptorHeap> m_resourceDescriptors;
    std::vector<ComPtr<ID3D12Resource>> m_texture;
    std::unique_ptr<SpriteBatch> m_spriteBatch;

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
    class CheckError& m_error;
};