#pragma once
#include <list>
#include <DirectXMath.h>

using namespace DirectX;

//  当たり判定の管理をするクラス
class CollisionManager
{
public:
    CollisionManager();
    ~CollisionManager();

    //  更新
    void Update();

    //  当たり判定を持つクラスを設定する
    void SetUpCollision(class CollisionObject* _class);
private:
    //  衝突判定を行う
    void DetectCollided(CollisionObject* _firstClass, CollisionObject* _secondClass);

    //  二つの球状オブジェクトが衝突したか
    bool IsCollide(const struct SphereData _firstSphere,const XMFLOAT3& _firstSpherePos, const  struct SphereData _secondSphere, const XMFLOAT3& _secondSpherePos);
    //  球状オブジェクトと箱型オブジェクトが衝突したか
    //  NOTE: 現状はビームとプレイヤーおよびビームと回避判定のみのため、
    //        処理負荷軽減のため、2D扱い(矩形と円)で判定を行う
    bool IsCollide(const struct SphereData _sphereData, const XMFLOAT3& _spherePos, const  struct BoxData _boxData, const XMFLOAT3& _boxPos);

    //  回転移動させた頂点を算出
    XMFLOAT2 CalculatePosRotationalMovement(const float _posX, const float _posY,
        const float _rotationReferencePointX, const float _rotationReferencePointY,
        const float _tiltAngle);

    //  辺と円は重なっているか
    bool IsOverlapCircleAndSide(const float _startPointPosX, const float _startPointPosY,
        const float _endPointPosX, const float _endPointPosY,
        const float _sideVecX, const float _sideVecY,
        const float _vecXStartPointToCircle, const  float _vecYStartPointToCircle,
        const float _vecXEndPointToCircle, const float _vecYEndPointToCircle,
        const float _circlePosX, const float _circlePosY, const float _circleRadius);

    //  ベクトルから見て右側に円はあるか
    //  NOTE: 矩形を構成する辺に沿って時計回りに定義したベクトルから見て、
    //        右側に円がある状況が全てのベクトルに当てはまる場合は、
    //        矩形の内部に円が完全に入っているとみなす
    bool IsCircleRightOfVec(const float _sideVecX, const float _sideVecY, 
        const float _vecXStartPointToCircle, const  float _vecYStartPointToCircle);

    //  当たり判定を持つクラスを格納する配列
    class CollisionObject* m_player;
    std::list<CollisionObject*> m_enemy;
    std::list<CollisionObject*> m_enemyAttack;
    std::list<CollisionObject*> m_beamAttack;
    std::list<CollisionObject*> m_enemyDefeater;
    class CollisionObject* m_dodgeRange;
    std::list<CollisionObject*> m_enemyDefeaterActivateEffect;
};