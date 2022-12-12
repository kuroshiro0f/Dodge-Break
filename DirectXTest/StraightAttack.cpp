#include "StraightAttack.h"

#include "ObjectTagName.h"
#include "EnemyAttackType.h"
#include "XMFLOATHelper.h"
#include "PMDModelType.h"
#include "Timer.h"
#include "EffectType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

StraightAttack::StraightAttack()
    :EnemyAttackBase(EnemyAttackType::Straight)
    , m_aliveTimer(new Timer())
    , m_unitVec({ 0,0,0 })
    , m_returnPoolFunc(nullptr)
    , m_petternID(0)
    , m_deltaTime(0)
    , m_param({})
{
}

StraightAttack::~StraightAttack()
{
    delete m_aliveTimer;
}

//  更新
void StraightAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    if (m_isDisplay)
    {
        //  生存時間が終わったら、攻撃を終える
        if (m_aliveTimer->GetElapseTime() * m_deltaTime >= m_param[m_petternID].aliveTime)
        {
            Delete();
        }
        //  移動
        Move();
        //  モデルの描画
        m_bulletModel.Draw(m_pos, PMDModelType::AimingBullet);
    }
}

//  発射
void StraightAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
    , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_petternID = _petternID;   //  パターンIDの設定
    m_pos = _pos;   //  座標
    m_unitVec = _unitVec;   //  発射方向の決定
    m_num = _num;       //  自分の番号
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isDisplay = true;     //  画面上に登場させる
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ

    //  生存時間計測開始
    m_aliveTimer->Start();
}

//  消去
void StraightAttack::Delete()
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
void StraightAttack::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    int num = fileData.GetArraySize(JsonDataType::StraightAttack, "StraightAttackData");
    for (int i = 0; i < num; i++)
    {
        Param temp;
        //  外部ファイルのデータを元にパラメータを決定
        temp.aliveTime = fileData.GetFloatData(JsonDataType::StraightAttack, "StraightAttackData", i, "AliveTime");
        temp.shootAngle = fileData.GetFloatData(JsonDataType::StraightAttack, "StraightAttackData", i, "ShootAngle");
        temp.speed = fileData.GetFloatData(JsonDataType::StraightAttack, "StraightAttackData", i, "Speed");
        m_param.emplace_back(temp);
    }

    //  衝突判定用の半径
    m_sphereData->radius = fileData.GetFloatData(JsonDataType::StraightAttack, "Radius");
}

//  衝突時の処理
void StraightAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  プレイヤーと衝突したら攻撃を消す
    if (_class.GetObjectTagName() == ObjectTagName::Player)
    {
        Delete();
    }
}

//  移動
void StraightAttack::Move()
{
    //  各方向への加速度を決定
    float speedX = m_unitVec.x * m_param[m_petternID].speed * m_deltaTime;
    float speedZ = m_unitVec.z * m_param[m_petternID].speed * m_deltaTime;

    //  加速度をもとに座標を決定
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, { speedX,0,speedZ });
}