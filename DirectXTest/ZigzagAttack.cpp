#include "ZigzagAttack.h"

#include "ObjectTagName.h"
#include "EnemyAttackType.h"
#include "XMFLOATHelper.h"
#include "PMDModelType.h"
#include "EffectType.h"
#include "Timer.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

ZigzagAttack::ZigzagAttack()
    :EnemyAttackBase(EnemyAttackType::Zigzag)
    , m_aliveTimer(new Timer())
    , m_changeTimer(new Timer())
    , m_shootTimer(new Timer())
    , m_unitVec({ 0,0,0 })
    , m_state(ZigzagState::NoShoot)
    , m_returnPoolFunc(nullptr)
    , m_deltaTime(0)
    , m_petternID(0)
    , m_param({})
{
}

ZigzagAttack::~ZigzagAttack()
{
    delete m_aliveTimer;
    delete m_changeTimer;
    delete m_shootTimer;
}

//  更新
void ZigzagAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    //  発射中の処理
    if (m_state == ZigzagState::Shoot)
    {
        //  発射に時間差をつけるための処理
        //  未発射の時は、発射時間になったら発射する
        if (!m_isDisplay)
        {
            if (m_shootTimer->GetElapseTime() >= m_param[m_petternID].shootIntervalForNumber * m_num)
            {
                //  生存時間計測開始
                m_aliveTimer->Start();
                //  変化時間計測開始
                m_changeTimer->Start();
                //  表示開始
                m_isDisplay = true;
            }
        }
        //  発射後の処理
        else
        {
            //  生存時間が終われば削除
            if (m_aliveTimer->GetElapseTime() >= m_param[m_petternID].aliveTime)
            {
                m_state = ZigzagState::EndAliveTime;
                Delete();
            }

            //  移動
            Move();
            //  モデルの描画
            //  NOTE: ジグザグ攻撃は特殊な影を描画するため、
            //        攻撃のモデルと影のモデルは同じデータにまとめてあります
            m_bulletModel.Draw(m_pos, m_param[m_petternID].angle, PMDModelType::ZigzagBullet);
        }
    }
    //  衝突後の処理
    else if(m_state == ZigzagState::Hit)
    {
        //  生存時間が終われば削除
        if (m_aliveTimer->GetElapseTime() >= m_param[m_petternID].aliveTime)
        {
            m_state = ZigzagState::EndAliveTime;
            Delete();
        }

        //  移動
        Move();
        //  衝突後のモデルの描画
        //  NOTE: ジグザグ攻撃は特殊な影を描画するため、
        //        攻撃のモデルと影のモデルは同じデータにまとめてあります
        m_bulletModel.Draw(m_pos, m_param[m_petternID].angle, PMDModelType::HitZigzagBullet);
    }
}

//  発射
void ZigzagAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
    , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_petternID = _petternID;   //  パターンIDの設定
    m_pos = _pos;
    m_pos.x += m_param[m_petternID].addPosX * (_num - 1);
    m_pos.z += m_param[m_petternID].addPosZ * (_num - 1);
    m_unitVec = _unitVec;   //  単位ベクトル
    m_num = _num;       //  自分の番号
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isDisplay = false;     //  まだ画面上に表示させない
    m_state = ZigzagState::Shoot;      //  発射した
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ

    //  発射までの時間計測開始
    m_shootTimer->Start();
}

//  消去
void ZigzagAttack::Delete()
{
    if (m_isDisplay || m_state == ZigzagState::Hit)
    {
        //  画面上から消す
        m_isDisplay = false;

        m_state = ZigzagState::NoShoot;
        //  EnemyAttackPool内に戻る
        m_returnPoolFunc(this);
    }
}

//  外部ファイルから読み取ったデータをロード
void ZigzagAttack::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    int num = fileData.GetArraySize(JsonDataType::ZigzagAttack, "ZigzagAttackData");
    for (int i = 0; i < num; i++)
    {
        Param temp;
        //  外部ファイルのデータを元にパラメータを決定
        temp.aliveTime = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "AliveTime");
        temp.shootIntervalForNumber = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "ShootIntervalForNumber");
        temp.speed = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "Speed");
        temp.angle = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "Angle");
        temp.addPosX = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "AddPosX");
        temp.addPosZ = fileData.GetFloatData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "AddPosZ");
        m_param.emplace_back(temp);
    }

    //  衝突判定用の半径
    m_sphereData->radius = fileData.GetFloatData(JsonDataType::ZigzagAttack, "Radius");
}

//  衝突時の処理
void ZigzagAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  プレイヤーと衝突したら下部のモデルのみ消去
    //  NOTE: プレイヤーとの衝突後に「衝突した後のジグザグ攻撃」のモデルと差し替えて、
    //        衝突判定などを消し、生存時間が終わるまでモデルを表示します。
    if (_class.GetObjectTagName() == ObjectTagName::Player)
    {
        //  衝突判定などを消す
        m_isDisplay = false;
        m_state = ZigzagState::EndAliveTime;
        Delete();
    }
}

//  移動
void ZigzagAttack::Move()
{
    //  ベクトルの向きに従って移動
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale(m_unitVec, m_param[m_petternID].speed * m_deltaTime));
}