#include "CollisionManager.h"

#include "CollisionType.h"
#include "CollisionObject.h"

CollisionManager::CollisionManager()
    :m_player(nullptr)
    , m_dodgeRange(nullptr)
    , m_enemy({})
    , m_enemyAttack({})
    , m_enemyDefeater({})
    , m_enemyDefeaterActivateEffect({})
{
    //  処理なし
}

CollisionManager::~CollisionManager()
{
    //  処理なし
}

//  更新
void CollisionManager::Update()
{
    //  ループ回数カウント用
    int count = 0;

    for (auto enemyItr : m_enemy)
    {
        //  プレイヤーとエネミーの当たり判定
        CollideTwoSphere(m_player, enemyItr);
        //  エネミーと回避範囲の当たり判定
        CollideTwoSphere(enemyItr, m_dodgeRange);
    }
    for (auto enemyAttackItr : m_enemyAttack)
    {
        //  エネミーの攻撃とプレイヤーの当たり判定
        CollideTwoSphere(enemyAttackItr, m_player);
        //  エネミーの攻撃と回避範囲の当たり判定
        CollideTwoSphere(enemyAttackItr, m_dodgeRange);
    }
    for (auto EnemyDefeaterItr : m_enemyDefeater)
    {
        //  塔とプレイヤーの当たり判定
        CollideTwoSphere(EnemyDefeaterItr, m_player);
        //  塔と塔を起動するエフェクトの当たり判定
        for (auto EnemyDefeaterActivaterItr : m_enemyDefeaterActivateEffect)
        {
            CollideTwoSphere(EnemyDefeaterItr, EnemyDefeaterActivaterItr);
        }
    }
}

//  当たり判定のあるクラスを設定
void CollisionManager::SetUpCollision(CollisionObject* _class)
{
    switch (_class->GetType())
    {
    case CollisionType::Player:
        m_player = _class;
        break;
    case CollisionType::Enemy:
        m_enemy.emplace_back(_class);
        break;
    case CollisionType::EnemyAttack:
        m_enemyAttack.emplace_back(_class);
        break;
    case CollisionType::EnemyDefeater:
        m_enemyDefeater.emplace_back(_class);
        break;
    case CollisionType::DodgeRange:
        m_dodgeRange = _class;
        break;
    case CollisionType::EnemyDefeaterActivateEffect:
        m_enemyDefeaterActivateEffect.emplace_back(_class);
        break;
    default:
        break;
    }
}

//  二つの球の当たり判定
void CollisionManager::CollideTwoSphere(CollisionObject* _firstClass, CollisionObject* _secondClass)
{
    //  非表示のオブジェクトは当たり判定を無視
    if (!(_firstClass->IsDisplay()) || !(_secondClass->IsDisplay()))return;

    //  一つ目の球の座標
    float firstCirclePosX = _firstClass->GetPos().x;
    float firstCirclePosY = _firstClass->GetPos().y;
    float firstCirclePosZ = _firstClass->GetPos().z;
    //  一つ目の球の半径
    float firstCircleRadius = _firstClass->GetRadius();

    //  二つ目の球の座標
    float secondCirclePosX = _secondClass->GetPos().x;
    float secondCirclePosY = _secondClass->GetPos().y;
    float secondCirclePosZ = _secondClass->GetPos().z;
    //  二つ目の球の半径
    float secondCircleRadius = _secondClass->GetRadius();

    //  中心座標同士の距離を求めて二乗する
    float squaredX = (firstCirclePosX - secondCirclePosX) * (firstCirclePosX - secondCirclePosX);
    float squaredY = (firstCirclePosY - secondCirclePosY) * (firstCirclePosY - secondCirclePosY);
    float squaredZ = (firstCirclePosZ - secondCirclePosZ) * (firstCirclePosZ - secondCirclePosZ);
    //  両円の半径を足して二乗する
    float squaredRadius = (firstCircleRadius + secondCircleRadius) * (firstCircleRadius + secondCircleRadius);

    //  三平方の定理により、衝突の判定を行う
    if (squaredX + squaredY + squaredZ <= squaredRadius)
    {
        _firstClass->OnCollisionEnter(*_secondClass);
        _secondClass->OnCollisionEnter(*_firstClass);
    }
}
