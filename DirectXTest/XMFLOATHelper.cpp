#include "XMFLOATHelper.h"

#include <cmath>

using namespace XMFLOATHelper;

//  加算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Add(const XMFLOAT3& _vec, const float _val)
{
    XMFLOAT3 ret;
    ret.x = _vec.x + _val;
    ret.y = _vec.y + _val;
    ret.z = _vec.z + _val;
    return ret;
}

//  XMFLOAT3同士の加算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Add(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    XMFLOAT3 ret;
    ret.x = _v1.x + _v2.x;
    ret.y = _v1.y + _v2.y;
    ret.z = _v1.z + _v2.z;
    return ret;
}

//  減算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Sub(const XMFLOAT3& _vec, const float _val)
{
    XMFLOAT3 ret;
    ret.x = _vec.x - _val;
    ret.y = _vec.y - _val;
    ret.z = _vec.z - _val;
    return ret;
}

//  XMFLOAT3同士の減算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Sub(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    XMFLOAT3 ret;
    ret.x = _v1.x - _v2.x;
    ret.y = _v1.y - _v2.y;
    ret.z = _v1.z - _v2.z;
    return ret;
}

//  乗算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Scale(const XMFLOAT3& _vec, const float _val)
{
    XMFLOAT3 ret;
    ret.x = _vec.x * _val;
    ret.y = _vec.y * _val;
    ret.z = _vec.z * _val;
    return ret;
}

//  XMFLOAT3同士の乗算
XMFLOAT3 XMFLOATHelper::XMFLOAT3Scale(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    XMFLOAT3 ret;
    ret.x = _v1.x * _v2.x;
    ret.y = _v1.y * _v2.y;
    ret.z = _v1.z * _v2.z;
    return ret;
}

//  逆ベクトル
XMFLOAT3 XMFLOATHelper::XMFLOAT3Inverse(const XMFLOAT3& _vec)
{
    XMFLOAT3 ret;
    ret.x = -_vec.x;
    ret.y = -_vec.y;
    ret.z = -_vec.z;
    return ret;
}

//  二点間の距離
float XMFLOATHelper::XMFLOAT3Distance(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    float ret;
    ret = pow((_v2.x - _v1.x), 2.0f) + pow((_v2.y - _v1.y), 2.0f) + pow((_v2.z - _v1.z), 2.0f);
    return sqrt(ret);
}

//  内積
float XMFLOATHelper::XMFLOAT3Inner(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    float ret;
    ret = _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
    return ret;
}

//  なす角θ
float XMFLOATHelper::XMFLOAT3Angle(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    float ret;
    float molecule = XMFLOAT3Inner(_v1, _v2);
    float denominator = sqrt(XMFLOAT3Inner(_v1, _v1)) * sqrt(XMFLOAT3Inner(_v2, _v2));
    ret = acos(molecule / denominator);
    return ret;
}

//  v1からv2への単位ベクトル
XMFLOAT3 XMFLOATHelper::XMFLOAT3Normalize(const XMFLOAT3& _v1, const XMFLOAT3& _v2)
{
    XMFLOAT3 ret;
    XMFLOAT3 vec = XMFLOAT3Sub(_v2, _v1);
    float distance = XMFLOAT3Distance(_v1, _v2);
    ret = XMFLOAT3Scale(vec, 1 / distance);
    return ret;
}

//  vecをxy平面上で角度_angle分傾けたときのベクトル
XMFLOAT3 XMFLOATHelper::XMFLOAT3TiltXY(const XMFLOAT3& _vec, const float _angle)
{
    XMFLOAT3 ret = _vec;
    ret.x = -sin(_angle) * _vec.y + cos(_angle) * _vec.x;
    ret.y = sin(_angle) * _vec.x + cos(_angle) * _vec.y;
    return ret;
}

//  vecをxz平面上で角度_angle分傾けたときのベクトル
XMFLOAT3 XMFLOATHelper::XMFLOAT3TiltXZ(const XMFLOAT3& _vec, const float _angle)
{
    XMFLOAT3 ret = _vec;
    ret.z = -sin(_angle) * _vec.x + cos(_angle) * _vec.z;
    ret.x = sin(_angle) * _vec.z + cos(_angle) * _vec.x;
    return ret;
}

//  vecをyz平面上で角度_angle分傾けたときのベクトル
XMFLOAT3 XMFLOATHelper::XMFLOAT3TiltYZ(const XMFLOAT3& _vec, const float _angle)
{
    XMFLOAT3 ret = _vec;
    ret.y = -sin(_angle) * _vec.z + cos(_angle) * _vec.y;
    ret.z = sin(_angle) * _vec.y + cos(_angle) * _vec.z;
    return ret;
}

//  ベジェ曲線
XMFLOAT3 XMFLOATHelper::XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate)
{
    XMFLOAT3 ret;
    ret.x = (1.0f - _rate) * _start.x + _rate * _end.x;
    ret.y = (1.0f - _rate) * _start.y + _rate * _end.y;
    ret.z = (1.0f - _rate) * _start.z + _rate * _end.z;
    return ret;
}
XMFLOAT3 XMFLOATHelper::XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate, const XMFLOAT3& _controlPoint)
{
    XMFLOAT3 ret;
    ret.x = pow((1.0f - _rate), 2.0f) * _start.x + 2.0f * (1.0f - _rate) * _rate * _controlPoint.x + pow(_rate, 2.0f) * _end.x;
    ret.y = pow((1.0f - _rate), 2.0f) * _start.y + 2.0f * (1.0f - _rate) * _rate * _controlPoint.y + pow(_rate, 2.0f) * _end.y;
    ret.z = pow((1.0f - _rate), 2.0f) * _start.z + 2.0f * (1.0f - _rate) * _rate * _controlPoint.z + pow(_rate, 2.0f) * _end.z;
    return ret;
}
XMFLOAT3 XMFLOATHelper::XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate, const XMFLOAT3& _controlPoint1, const XMFLOAT3& _controlPoint2)
{
    XMFLOAT3 ret;
    ret.x = pow((1.0f - _rate), 3.0f) * _start.x + 3.0f * pow((1.0f - _rate), 2.0f) * _rate * _controlPoint1.x + 3.0f * (1.0f - _rate) * pow(_rate, 2.0f) * _controlPoint2.x + pow(_rate, 3.0f) * _end.x;
    ret.y = pow((1.0f - _rate), 3.0f) * _start.y + 3.0f * pow((1.0f - _rate), 2.0f) * _rate * _controlPoint1.y + 3.0f * (1.0f - _rate) * pow(_rate, 2.0f) * _controlPoint2.y + pow(_rate, 3.0f) * _end.y;
    ret.x = pow((1.0f - _rate), 3.0f) * _start.z + 3.0f * pow((1.0f - _rate), 2.0f) * _rate * _controlPoint1.z + 3.0f * (1.0f - _rate) * pow(_rate, 2.0f) * _controlPoint2.z + pow(_rate, 3.0f) * _end.z;
    return ret;
}

//  同値か判定
bool XMFLOATHelper::IsEqualXMFLOAT3(const XMFLOAT3& _pos1, const XMFLOAT3& _pos2)
{
    if (_pos1.x == _pos2.x && 
        _pos1.y == _pos2.y && 
        _pos1.z == _pos2.z)
    {
        return true;
    }
    return false;
}
