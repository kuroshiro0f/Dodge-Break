#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  座標変換を行うヘルパー名前空間
namespace CoordinateTransformation 
{
    //  ワールド座標をスクリーン座標に変換
    XMFLOAT2 CalcWorldToScreen(const XMFLOAT3& _pos,const float _windowWidth,const float _windowHeight,const XMMATRIX& _view,const XMMATRIX& _proj);

    //  スクリーンの座標をワールド座標に変換
    XMFLOAT3 CalcScreenToWorld(const XMFLOAT3& _pos, const float _windowWidth, const float _windowHeight, const XMMATRIX& _view, const XMMATRIX& _proj);

    //  スクリーン座標とXZ平面のワールド座標交点算出
    XMFLOAT3 CalcScreenPosToXZ(const XMFLOAT2& _pos, const float _windowWidth, const float _windowHeight, const XMMATRIX& _view, const XMMATRIX& _proj);
};