#if _DEBUG
#include "DebugAttack.h"

#include "CollisionType.h"
#include "EnemyAttackType.h"
#include "DebugAttackParam.h"
#include "PMDModelType.h"
#include "XMFLOATHelper.h"
#include "EffectType.h"
#include "Timer.h"

#include "Singleton.h"
#include "PMDModel.h"

DebugAttack::DebugAttack()
    :EnemyAttackBase(EnemyAttackType::Debug)
    , m_aliveTimer(new Timer())
    , m_changeTimer(new Timer())
    , m_unitVec({ 0,0,0 })
    , m_count(0)
    , m_deltaTime(0)
    , m_returnPoolFunc(nullptr)
{
    //  各パラメータの初期化
    m_pos = { 0,0,0 };  //  座標
    //m_radius = DebugAttackParam::RADIUS;   //  衝突判定用の半径
    m_isDisplay = false;    //  最初から登場はしていない
}

DebugAttack::~DebugAttack()
{
    delete m_aliveTimer;
    delete m_changeTimer;
}

void DebugAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    if (m_isDisplay)
    {
        if (m_aliveTimer->GetElapseTime() >= DebugAttackParam::ALIVE_TIME)
        {
            Delete();
        }

        ////  移動
        //Move();
        ////  攻撃自体の見た目となるエフェクトの再生
        //m_effect.PlayEffect(m_effectType, m_pos);
        
        //for (auto itr : m_effectPos)
        //{
        //    m_effect.PlayEffect(m_effectType, itr);
        //}
    }
}

void DebugAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
    , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_pos = _pos;
    //m_pos = { _pos.x,_pos.y - (_num * 10.0f),_pos.z };   //  座標
    m_unitVec = _unitVec;   //  単位ベクトル
    m_num = _num;       //  自分の番号
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isDisplay = true;     //  画面上に登場させる
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ

    m_count = 0.2f;

    //  生存時間計測開始
    m_aliveTimer->Start();
    //  変化時間計測開始
    m_changeTimer->Start();
}

void DebugAttack::Delete()
{
    //  画面上から消す
    m_isDisplay = false;
    //  EnemyAttackPool内に戻る
    m_returnPoolFunc(this);
}

void DebugAttack::OnCollisionEnter(const CollisionObject& _class)
{
}

void DebugAttack::Move()
{
    //  中心にある弾の周りを二つの弾が回りながら飛んでいく攻撃
    /*m_count += 0.2f;
    switch (m_num)
    {
    case 0:
        m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale({ cos(m_count - PI),sin(m_count - PI),0.2f }, 2.0f));
        break;
    case 1:
        m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale(m_unitVec, 0.4f));
        break;
    case 2:
        m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale({ cos(m_count),sin(m_count),0.2f }, 2.0f));
        break;
    default:
        break;
    }*/

    //  回りながら飛ぶ弾
    /*m_count += 0.2f;
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale({ cos(m_count),sin(m_count),0.2f }, 2.0f));*/

    //  斜め整列
    /*  m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, m_unitVec);
    for (int i = 0; i < 10; i++)
    {
        m_effectPos[i] = { m_pos.x + i * 3.0f,m_pos.y + i * 3.0f, m_pos.z };
    }*/
}
#endif