#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  XMFLOATのヘルパー
//  NOTE: XMFLOAT型には加算や乗算、正規化などをする機能がなく、
//        そのままだとDirectX関連の面倒な関数を複数使わないと計算できなかったため、
//        ヘルパークラスを用意しました。
namespace XMFLOATHelper
{
    //  加算
    XMFLOAT3 XMFLOAT3Add(const XMFLOAT3& _vec, const float _val);
    XMFLOAT3 XMFLOAT3Add(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  減算
    XMFLOAT3 XMFLOAT3Sub(const XMFLOAT3& _vec, const float _val);
    XMFLOAT3 XMFLOAT3Sub(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  乗算
    XMFLOAT3 XMFLOAT3Scale(const XMFLOAT3& _vec, const float _val);
    XMFLOAT3 XMFLOAT3Scale(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  逆ベクトル
    XMFLOAT3 XMFLOAT3Inverse(const XMFLOAT3& _vec);
    //  二点間の距離
    float XMFLOAT3Distance(const XMFLOAT3& _v1,const XMFLOAT3& _v2);
    //  内積
    float XMFLOAT3Inner(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  なす角θ
    float XMFLOAT3Angle(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  v1からv2への単位ベクトル
    XMFLOAT3 XMFLOAT3Normalize(const XMFLOAT3& _v1, const XMFLOAT3& _v2);
    //  vecをxy平面上で角度_angle分傾けたときのベクトル
    XMFLOAT3 XMFLOAT3TiltXY(const XMFLOAT3& _vec, const float _angle);
    //  vecをxz平面上で角度_angle分傾けたときのベクトル
    XMFLOAT3 XMFLOAT3TiltXZ(const XMFLOAT3& _vec, const float _angle);
    //  vecをyz平面上で角度_angle分傾けたときのベクトル
    XMFLOAT3 XMFLOAT3TiltYZ(const XMFLOAT3& _vec, const float _angle);
    //  ベジェ曲線
    XMFLOAT3 XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate);
    XMFLOAT3 XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate, const XMFLOAT3& _controlPoint);
    XMFLOAT3 XMFLOAT3CreateBezierCurve(const XMFLOAT3& _start, const XMFLOAT3& _end, const float _rate, const XMFLOAT3& _controlPoint1, const XMFLOAT3& _controlPoint2);
    //  同値か判定
    bool IsEqualXMFLOAT3(const XMFLOAT3& _pos1, const XMFLOAT3& _pos2);
}