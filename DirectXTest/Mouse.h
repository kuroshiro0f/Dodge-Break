#pragma once
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

//  マウスの座標を得るクラス
class Mouse {
public:
    Mouse();
    ~Mouse();

    //  初期化
    void Init();

    //  更新
    void Update();

    //  マウスのスクリーン座標を得る
    XMFLOAT2 GetMouseScreenPos();

    //  マウスのワールド座標を得る
    XMFLOAT3 GetMouseWorldPos();
private:
    //  マウスの位置
    POINT m_point;

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
};