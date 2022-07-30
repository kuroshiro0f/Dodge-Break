#include "CollisionDetection.h"

CollisionDetection::CollisionDetection()
{
    //  処理なし
}

CollisionDetection::~CollisionDetection()
{
    //  処理なし
}

//  点と矩形の当たり判定
bool CollisionDetection::CollidePointAndRect(const XMFLOAT2& _pointPos, const XMFLOAT2& _rectPos, const float _width, const float _height)
{
    //  点の座標
    float pointPosX = _pointPos.x;
    float pointPosY = _pointPos.y;

    //  矩形の座標
    float rectPosX = _rectPos.x;
    float rectPosY = _rectPos.y;

    //  矩形の内部に点の座標が存在するかで判定
    if (pointPosX >= rectPosX && pointPosX <= rectPosX + _width
        && pointPosY >= rectPosY && pointPosY <= rectPosY + _height)
    {
        return true;
    }
    return false;
}