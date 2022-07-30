#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  当たり判定の計算をするクラス
class CollisionDetection
{
public:
    CollisionDetection();
    ~CollisionDetection();

    //  点と矩形の当たり判定
    bool CollidePointAndRect(const XMFLOAT2& _pointPos, const XMFLOAT2& _rectPos, const float _width, const float _height);
};
