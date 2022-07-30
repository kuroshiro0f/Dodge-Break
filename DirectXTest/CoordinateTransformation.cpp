#include "CoordinateTransformation.h"

//  ワールド座標をスクリーン座標に変換
XMFLOAT2 CoordinateTransformation::CalcWorldToScreen(const XMFLOAT3& _pos, const float _windowWidth, const float _windowHeight, const XMMATRIX& _view, const XMMATRIX& _proj)
{
    //  ビューポート行列を作る
    XMMATRIX VP = XMMatrixIdentity();
    VP.r[0].m128_f32[0] = _windowWidth * 0.5f;
    VP.r[1].m128_f32[1] = -_windowHeight * 0.5f;
    VP.r[3].m128_f32[0] = _windowWidth * 0.5f;
    VP.r[3].m128_f32[1] = _windowHeight * 0.5f;

    //  ビュー行列、プロジェクション行列、ビューポート行列を元にスクリーン座標を算出
    XMMATRIX temp = _view * _proj * VP;
    XMFLOAT3 pout = _pos;
    XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pout), temp);
    XMStoreFloat3(&pout, vec);
    return { pout.x,pout.y };
}

//  スクリーンの座標をワールド座標に変換
XMFLOAT3 CoordinateTransformation::CalcScreenToWorld(const XMFLOAT3& _pos, const float _windowWidth, const float _windowHeight, const XMMATRIX& _view, const XMMATRIX& _proj)
{
    //  ビュー行列の逆行列を得る
    XMMATRIX InvView = XMMatrixInverse(nullptr, _view);
    //  プロジェクション行列の逆行列を得る
    XMMATRIX InvPrj = XMMatrixInverse(nullptr, _proj);
    //  ビューポート行列を作る
    XMMATRIX VP = XMMatrixIdentity();
    VP.r[0].m128_f32[0] = _windowWidth * 0.5f;
    VP.r[1].m128_f32[1] = -_windowHeight * 0.5f;
    VP.r[3].m128_f32[0] = _windowWidth * 0.5f;
    VP.r[3].m128_f32[1] = _windowHeight * 0.5f;
    //  ビューポート行列の逆行列を得る
    XMMATRIX InvVP = XMMatrixInverse(nullptr, VP);

    //  ビュー行列、プロジェクション行列、ビューポート行列それぞれの逆行列をもとにワールド座標を算出
    XMMATRIX temp = InvVP * InvPrj * InvView;
    XMFLOAT3 pout = _pos;
    XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pout), temp);
    XMStoreFloat3(&pout, vec);
    return pout;
}

//  スクリーン座標とXZ平面のワールド座標交点算出
XMFLOAT3 CoordinateTransformation::CalcScreenPosToXZ(const XMFLOAT2& _pos, const float _windowWidth, const float _windowHeight, const XMMATRIX& _view, const XMMATRIX& _proj)
{
    //  Y座標が最も近くなる最近点を指定
    XMFLOAT3 nearPos = CalcScreenToWorld({ _pos.x,_pos.y,0.0f }, _windowWidth, _windowHeight, _view, _proj);
    //  Y座標が最も遠くなる最遠点を指定
    XMFLOAT3 farPos = CalcScreenToWorld({ _pos.x,_pos.y,1.0f }, _windowWidth, _windowHeight, _view, _proj);
    //  最近点から最遠点の単位ベクトルを視線ベクトルとして定義
    XMVECTOR rayVec = XMLoadFloat3(&farPos) - XMLoadFloat3(&nearPos);
    rayVec = XMVector3Normalize(rayVec);
    XMFLOAT3 ray;
    XMStoreFloat3(&ray, rayVec);
    //  視線ベクトルが地面(Y = 0の平面)と交差する場合
    if (ray.y <= 0)
    {
        //  視線ベクトルと床の交点を算出し、その交点の座標を返す
        XMFLOAT3 normal = { 0,-1.0f,0 };
        XMVECTOR rayDot = XMVector3Dot(rayVec, XMLoadFloat3(&normal));
        nearPos = { -nearPos.x,-nearPos.y,-nearPos.z };
        XMVECTOR nearDot = XMVector3Dot(XMLoadFloat3(&nearPos), XMLoadFloat3(&normal));
        XMFLOAT3 Lray3, LP03;
        XMStoreFloat3(&Lray3, rayDot);
        XMStoreFloat3(&LP03, nearDot);
        float Lray = Lray3.x + Lray3.y + Lray3.z;
        float LP0 = LP03.x + LP03.y + LP03.z;
        nearPos = { -nearPos.x,-nearPos.y,-nearPos.z };
        XMVECTOR nearVec = XMLoadFloat3(&nearPos);
        XMVECTOR vec = nearVec + (LP0 / Lray) * rayVec;
        XMFLOAT3 pos;
        XMStoreFloat3(&pos, vec);
        return pos;
    }
    else
    {
        //  最遠点を返す
        return farPos;
    }
}
