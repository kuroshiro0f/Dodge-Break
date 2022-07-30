#include "FannelAttack.h"

#include "CollisionType.h"
#include "EnemyAttackType.h"
#include "XMFLOATHelper.h"
#include "PMDModelType.h"
#include "Timer.h"
#include "EffectType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

FannelAttack::FannelAttack()
    :EnemyAttackBase(EnemyAttackType::Fannel)
    , m_aliveTimer(new Timer())
    , m_changeTimer(new Timer())
    , m_unitVec({ 0,0,0 })
    , m_deltaTime(0)
    , m_moveState(MoveState::None)
    , m_petternID(0)
    , m_param({})
    , m_returnPoolFunc(nullptr)
{
}

FannelAttack::~FannelAttack()
{
    delete m_aliveTimer;
    delete m_changeTimer;
}

//  更新
void FannelAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    if (m_isDisplay)
    {
        switch (m_moveState)
        {
        case FannelAttack::MoveState::Straight:
            //  停止する時間になったら移動を停止
            if (m_changeTimer->GetElapseTime() >= m_param[m_petternID].timeToStop)
            {
                m_unitVec = { 0,0,0 };
                m_moveState = MoveState::Stop;
                m_changeTimer->Start();
            }
            break;
        case FannelAttack::MoveState::Stop:
            //  移動開始時間になったら追尾移動を開始
            if (m_changeTimer->GetElapseTime() >= m_param[m_petternID].timeAfterStop)
            {
                m_unitVec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, _targetPos);

                m_moveState = MoveState::Track;
            }
            break;
        case FannelAttack::MoveState::Track:
            //  生存時間終了または地面に攻撃が衝突したら攻撃を削除
            if (m_aliveTimer->GetElapseTime() >= m_param[m_petternID].aliveTime || m_pos.y < 0)
            {
                Delete();
            }
            break;
        default:
            break;
        }
        //  移動
        Move();
        //  モデルの描画
        m_bulletModel.Draw(m_pos, PMDModelType::UniqueBullet);
    }
}

//  発射
void FannelAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
    , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_petternID = _petternID;   //  パターンIDの設定
    m_pos = _pos;   //  座標
    m_unitVec = _unitVec;   //  単位ベクトル
    m_num = _num;       //  自分の番号
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isDisplay = true;     //  画面上に登場させる
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ
    
    //  まずはまっすぐ飛ぶ
    m_moveState = MoveState::Straight;

    //  生存時間計測開始
    m_aliveTimer->Start();
    //  変化時間計測開始
    m_changeTimer->Start();
}

//  消去
void FannelAttack::Delete()
{
    if (m_isDisplay)
    {
        //  画面上から消す
        m_isDisplay = false;
        //  EnemyAttackPool内に戻る
        m_returnPoolFunc(this);
    }
}

//  外部ファイルから読み取ったデータをロード
void FannelAttack::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    int num = fileData.GetArraySize(JsonDataType::FannelAttack, "FannelAttackData");
    for (int i = 0; i < num; i++)
    {
        Param temp;
        //  外部ファイルのデータを元にパラメータを決定
        temp.aliveTime = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "AliveTime");
        temp.shootIntervalForNumber = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "ShootIntervalForNumber");
        temp.trackinigRate = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "TrackingRate");
        temp.timeToStop = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "TimeToStop");
        temp.speedToStop = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "SpeedToStop");
        temp.timeAfterStop = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "TimeAfterStop");
        temp.speedAfterStop = fileData.GetFloatData(JsonDataType::FannelAttack, "FannelAttackData", i, "SpeedAfterStop");
        m_param.emplace_back(temp);
    }

    //  衝突判定用の半径
    m_radius = fileData.GetFloatData(JsonDataType::FannelAttack, "Radius");
}

//  衝突時の処理
void FannelAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  プレイヤーと衝突したら攻撃を消す
    if (_class.GetType() == CollisionType::Player)
    {
        Delete();
    }
}

//  移動
void FannelAttack::Move()
{
    //  移動量を保存する変数
    float speedX = 0;
    float speedY = 0;
    float speedZ = 0;
    switch (m_moveState)
    {
    case FannelAttack::MoveState::Straight:
        //  各方向への移動量を計算
        speedX = m_unitVec.x * m_param[m_petternID].speedToStop * m_deltaTime;
        speedY = m_unitVec.y * m_param[m_petternID].speedToStop * m_deltaTime;
        speedZ = m_unitVec.z * m_param[m_petternID].speedToStop * m_deltaTime;
        break;
    case FannelAttack::MoveState::Stop:
        //  停止中なので移動量は0
        speedX = 0;
        speedY = 0;
        speedZ = 0;
        break;
    case FannelAttack::MoveState::Track:
        //  各方向への移動量を計算
        speedX = m_unitVec.x * m_param[m_petternID].speedAfterStop * m_deltaTime;
        speedY = m_unitVec.y * m_param[m_petternID].speedAfterStop * m_deltaTime;
        speedZ = m_unitVec.z * m_param[m_petternID].speedAfterStop * m_deltaTime;
        break;
    default:
        break;
    }

    //  移動量をもとに座標を決定
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, { speedX,speedY,speedZ });
}