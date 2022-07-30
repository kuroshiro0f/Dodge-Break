#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  定数
#define PI    3.1415926f        //  円周率

enum class CollisionType;
//  当たり判定のあるオブジェクトの親クラス
class CollisionObject
{
public:
    CollisionObject();
    virtual ~CollisionObject() {};

    //  初期化
    void Init(const CollisionType& _type);

    //  衝突した時に呼び出される関数
    virtual void OnCollisionEnter(const CollisionObject& _class) = 0;

    //  ゲッター
    XMFLOAT3 GetPos()const { return m_pos; }
    float GetRadius()const { return m_radius; }
    CollisionType GetType()const { return m_type; }
    bool IsDisplay()const { return m_isDisplay; }
protected:
    //  座標
    XMFLOAT3 m_pos;
    //  半径
    float m_radius;

    //  渡すクラスのタイプ
    CollisionType m_type;

    //  状態
    bool m_isDisplay;
};