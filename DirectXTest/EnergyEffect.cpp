#include "EnergyEffect.h"

#include "CollisionType.h"
#include "EnemyType.h"
#include "EnemyDefeaterMapper.h"
#include "EnumIterator.h"
#include "XMFLOATHelper.h"
#include "EffectType.h"
#include "EventNotificator.h"
#include "EventType.h"
#include "Timer.h"

#include "Singleton.h"
#include "EffekseerManager.h"

//  1フレームでの移動割合増加量
const float MOVE_SPEED = 0.05f;
//  移動割合の最高値
const float MAX_MOVE_RATE = 1.0f;

//  角度計算の基準
const XMFLOAT3 POSITIVE_DIRECTION = { 0,0,1.0f };

//  拡散時間
const float SPREAD_TIME = 70.0f;
//  エネルギーが満タンの時の拡散時間
const float SPREAD_TIME_FOR_ENERGY_MAX = 70.0f;

EnergyEffect::EnergyEffect()
    :m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_enemyDefeaterMapperClass(nullptr)
    , m_eventClass(nullptr)
    , m_spreadTimer(new Timer())
    , m_firstPos({ 0,0,0 })
    , m_moveRate(0)
    , m_angle(0)
    , m_deltaTime(0)
    , m_effectHandle({})
    , m_state(State::NoDispay)
    , m_isEnergyMax(false)
    , m_pos({})
    , m_controlPoint({})
{
    //  処理なし
}

EnergyEffect::~EnergyEffect()
{
    delete m_spreadTimer;
}

//  初期化
void EnergyEffect::Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass)
{
    //  塔の位置を把握するためのクラスのインスタンスを取得
    m_enemyDefeaterMapperClass = _enemyDefeaterMapperClass;
    //  イベント通知クラスのインスタンスを取得
    m_eventClass = _eventClass;
}

//  リセット
void EnergyEffect::Reset()
{
    //  エフェクトをリセット
    for (auto itr : m_effectHandle)
    {
        m_effect.StopEffect(itr.second);
        itr.second = 0;
    }
    //  座標をリセット
    for (auto itr : m_pos)
    {
        itr.second = { 0,0,0 };
    }
    //  非表示に
    m_state = State::NoDispay;
    //  移動割合を0に
    m_moveRate = 0;
    //  時間計測クラスのリセット
    m_spreadTimer->Reset();
}

//  更新
void EnergyEffect::Update(const float _deltaTime)
{
    //  デルタタイムの更新
    m_deltaTime = _deltaTime;

    if (m_state != State::NoDispay)
    {
        //  移動
        Move();

        if (m_state == State::Spread && m_spreadTimer->GetElapseTime() >= SPREAD_TIME && !m_isEnergyMax)
        {
            m_state = State::GoTower;
            m_moveRate = 0;
        }
        else if (m_state == State::Spread && m_spreadTimer->GetElapseTime() >= SPREAD_TIME_FOR_ENERGY_MAX && m_isEnergyMax)
        {
            m_state = State::NoDispay;
        }

        //  列挙型EnemyTypeのイテレータを作成
        typedef EnumIterator<EnemyType, EnemyType::AimingShootEnemy, EnemyType::UniqueShootEnemy> typeItr;
        for (auto itr : typeItr())
        {
            //  エフェクトの更新
            m_effectHandle[itr] = m_effect.UpdateLoopPlayEffect(m_pos[itr], m_effectHandle[itr], m_deltaTime, { 0,m_angle[itr],0 });
        }
        if (m_moveRate >= MAX_MOVE_RATE && m_state == State::GoTower)
        {
            m_eventClass->Notificate(EventType::HitEnergyEffect);
            m_state = State::NoDispay;
        }
    }
}

//  発射
void EnergyEffect::Shoot(const XMFLOAT3 _pos, const bool _isEnergyMax)
{
    //  列挙型EnemyTypeのイテレータを作成
    typedef EnumIterator<EnemyType, EnemyType::AimingShootEnemy, EnemyType::UniqueShootEnemy> typeItr;

    //  移動割合をリセット
    m_moveRate = 0;
    //  出現させる
    m_state = State::Spread;

    //  発射時の座標をセット
    m_firstPos = _pos;
    for (auto itr : typeItr())
    {
        //  座標をセット
        m_pos[itr] = m_firstPos;
        //  制御点を設定
        m_controlPoint[itr] = { m_enemyDefeaterMapperClass->GetEnemyDefeaterPos(itr).x,0,m_pos[itr].z };
        //  エネルギーが満タンか否かでエフェクトを変更
        if (_isEnergyMax)
        {
            //  エネルギーは満タン
            m_isEnergyMax = true;
            //  エフェクトを再生
            m_effectHandle[itr] = m_effect.PlayEffect(EffectType::DodgeEnergyForEnergyMax, m_pos[itr], false);
        }
        else
        {
            //  エネルギーは満タンではない
            m_isEnergyMax = false;
            //  エフェクトを再生
            m_effectHandle[itr] = m_effect.PlayEffect(EffectType::DodgeEnergy, m_pos[itr], false);
        }
    }
    //  拡散時間計測開始
    m_spreadTimer->Start();
}

//  移動
void EnergyEffect::Move()
{
    //  列挙型EnemyTypeのイテレータを作成
    typedef EnumIterator<EnemyType, EnemyType::AimingShootEnemy, EnemyType::UniqueShootEnemy> typeItr;
    for (auto itr : typeItr())
    {
        if (m_state == State::GoTower)
        {
            //  ベジェ曲線での計算を用いて塔に移動
            //  NOTE: 距離に関係なく塔に到着するスピードを一律にするために
            //        移動の計算にはベジェ曲線を使っております。
            m_moveRate += MOVE_SPEED;
            if (m_moveRate >= MAX_MOVE_RATE)
            {
                m_moveRate = MAX_MOVE_RATE;
            }
            m_pos[itr] = XMFLOATHelper::XMFLOAT3CreateBezierCurve(m_firstPos, m_enemyDefeaterMapperClass->GetEnemyDefeaterPos(itr), m_moveRate);
        }
        //  角度計算用に現在座標から移動目標の座標までの単位ベクトルを取得
        XMFLOAT3 unitVec = XMFLOATHelper::XMFLOAT3Normalize(m_pos[itr], m_enemyDefeaterMapperClass->GetEnemyDefeaterPos(itr));
        //  角度の計算
        //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
        if (m_enemyDefeaterMapperClass->GetEnemyDefeaterPos(itr).x >= m_pos[itr].x)
        {
            m_angle[itr] = XMFLOATHelper::XMFLOAT3Angle(POSITIVE_DIRECTION, unitVec);
        }
        else
        {
            m_angle[itr] = -XMFLOATHelper::XMFLOAT3Angle(POSITIVE_DIRECTION, unitVec);
        }
    }
}