#include "LineAttack.h"

#include "ObjectTagName.h"
#include "EnemyAttackType.h"
#include "XMFLOATHelper.h"
#include "PMDModelType.h"
#include "Timer.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

LineAttack::LineAttack()
    :EnemyAttackBase(EnemyAttackType::Line)
    , m_aliveTimer(new Timer())
    , m_changeTimer(new Timer())
    , m_unitVec({ 0,0,0 })
    , m_speed(0)
    , m_deltaTime(0)
    , m_firstPos({ 0,0,0 })
    , m_petternID(0)
    , m_state(LineAttackState::None)
    , m_stopDistance(0)
    , m_returnPoolFunc(nullptr)
    , m_param({})
{
}

LineAttack::~LineAttack()
{
    delete m_aliveTimer;
    delete m_changeTimer;
}

//  更新
void LineAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    switch (m_state)
    {
    case LineAttack::LineAttackState::Shoot:
        //  時間になったら弾を発射
        //  NOTE: 弾毎に発射時間を変えています
        if (m_aliveTimer->GetElapseTime() >= m_param[m_petternID].shootIntervalForNumber * m_num)
        {
            m_isDisplay = true;
            m_speed = m_param[m_petternID].alignSpeed;
            m_state = LineAttackState::Align;
        }
        break;
    case LineAttack::LineAttackState::Align:
        //  一定距離移動したら弾を停止
        if (XMFLOATHelper::XMFLOAT3Distance(m_firstPos, m_pos) >= m_stopDistance)
        {
            m_unitVec = { 0,0,0 };
            m_state = LineAttackState::Stop;
        }
        break;
    case LineAttack::LineAttackState::Stop:
        //  一定時間経過したら弾の移動を再開
        if (m_changeTimer->GetElapseTime() >= m_param[m_petternID].timeForRestartMove)
        {
            m_unitVec = m_param[m_petternID].rollVec;
            m_speed = m_param[m_petternID].speed;
            m_state = LineAttackState::Move;
        }
        break;
    case LineAttack::LineAttackState::Move:
        //  生存時間が終われば弾を削除
        if (m_aliveTimer->GetElapseTime() >= m_param[m_petternID].aliveTime)
        {
            Delete();
        }
        break;
    default:
        break;
    }

    if (m_isDisplay)
    {
        //  移動
        Move();
        //  モデルの描画
        m_bulletModel.Draw(m_pos, PMDModelType::NoAimingBullet);
    }
}

//  発射
void LineAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num, float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_petternID = _petternID;   //  パターンIDの設定
    m_pos = _pos;       //  座標
    m_firstPos = m_pos; //  発射時の座標
    m_unitVec = _unitVec;       //  単位ベクトル
    m_num = _num;       //  自分の番号
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isDisplay = false;        //  まだ画面上に登場させない
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ
    m_state = LineAttackState::Shoot;   //  発射開始

    //  止まるまでの移動距離を計算
    m_stopDistance = m_param[m_petternID].stopDistance - (m_param[m_petternID].distanceIntervalForNumber * m_num);

    //  生存時間計測開始
    m_aliveTimer->Start();
    //  変化時間計測開始
    m_changeTimer->Start();
}

//  消去
void LineAttack::Delete()
{
    if (m_state != LineAttackState::NoShoot)
    {
        //  画面上から消す
        m_isDisplay = false;
        //  状態を未発射に
        m_state = LineAttackState::NoShoot;
        //  EnemyAttackPool内に戻る
        m_returnPoolFunc(this);
    }
}

//  外部ファイルから読み取ったデータをロード
void LineAttack::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    int num = fileData.GetArraySize(JsonDataType::LineAttack, "LineAttackData");
    for (int i = 0; i < num; i++)
    {
        Param temp;
        //  外部ファイルのデータを元にパラメータを決定
        temp.aliveTime = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "AliveTime");
        temp.alignSpeed = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "AlignSpeed");
        temp.speed = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "Speed");
        temp.shootIntervalForNumber = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "ShootIntervalForNumber");
        temp.stopDistance = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "StopDistance");
        temp.distanceIntervalForNumber = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "DistanceIntervalForNumber");
        temp.timeForRestartMove = fileData.GetFloatData(JsonDataType::LineAttack, "LineAttackData", i, "TimeForRestartMove");
        temp.rollVec = fileData.GetXMFLOAT3Data(JsonDataType::LineAttack, "LineAttackData", i, "RollVec");
        m_param.emplace_back(temp);
    }

    //  衝突判定用の半径
    m_sphereData->radius = fileData.GetFloatData(JsonDataType::LineAttack, "Radius");
}

//  衝突時の処理
void LineAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  プレイヤーと衝突したら攻撃を消す
    if (_class.GetObjectTagName() == ObjectTagName::Player)
    {
        Delete();
    }
}

//  移動
void LineAttack::Move()
{
    //  各方向への加速度を決定
    float speedX = m_unitVec.x * m_speed * m_deltaTime;
    float speedY = m_unitVec.y * m_speed * m_deltaTime;
    float speedZ = m_unitVec.z * m_speed * m_deltaTime;

    //  加速度をもとに座標を決定
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, { speedX,speedY,speedZ });
}
