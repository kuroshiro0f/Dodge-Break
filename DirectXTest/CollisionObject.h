#pragma once
#include <DirectXMath.h>

#include "SphereData.h"
#include "BoxData.h"

using namespace DirectX;

//  定数
#define PI    3.1415926f        //  円周率

enum class CollisionType;
enum class ObjectTagName;
//  当たり判定のあるオブジェクトの親クラス
class CollisionObject
{
public:
    CollisionObject();
    virtual ~CollisionObject() {};

    //  初期化
    void Init(const CollisionType& _collisionType,const ObjectTagName _objectTagName);

    //  衝突した時に呼び出される関数
    virtual void OnCollisionEnter(const CollisionObject& _class) = 0;

    //  ゲッター
    XMFLOAT3 GetPos()const { return m_pos; }        //  座標
    SphereData GetSphereData()const { return *m_sphereData; }   //  球状オブジェクトのデータ
    BoxData GetBoxData()const { return *m_boxData; }        //  箱型オブジェクトのデータ
    CollisionType GetCollisionType()const { return m_collisionType; }   //  オブジェクトの形の種類
    ObjectTagName GetObjectTagName()const { return m_objectTagName; }   //  オブジェクトの名前
    bool IsDisplay()const { return m_isDisplay; }   //  出現中か
protected:
    //  座標
    XMFLOAT3 m_pos;

    //  NOTE: 派生クラスは自分のオブジェクトの形に応じて、
    //        どちらかのデータに値を入れる
    //  球状オブジェクトのデータ
    SphereData* m_sphereData;
    //  箱型オブジェクトのデータ
    BoxData* m_boxData;

    //  渡すクラスのタイプ
    CollisionType m_collisionType;
    ObjectTagName m_objectTagName;

    //  状態
    bool m_isDisplay;
};