#include "SpreadAttack.h"

#include "ObjectTagName.h"
#include "EnemyAttackType.h"
#include "XMFLOATHelper.h"
#include "PMDModelType.h"
#include "Timer.h"
#include "EffectType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

SpreadAttack::SpreadAttack()
    :EnemyAttackBase(EnemyAttackType::Spread)
    , m_aliveTimer(new Timer())
    , m_firstPos({ 0,0,0 })
    , m_unitVec({ 0,0,0 })
    , m_spreadUnitVec({ 0,0,0 })
    , m_returnPoolFunc(nullptr)
    , m_isSpread(false)
    , m_petternID(0)
    , m_deltaTime(0)
    , m_param({})
{
}

SpreadAttack::~SpreadAttack()
{
    delete m_aliveTimer;
}

//  更新
void SpreadAttack::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  デルタタイム更新
    m_deltaTime = _deltaTime;

    if (m_isDisplay)
    {
        //  生存時間が終わったら、攻撃を終える
        if (m_aliveTimer->GetElapseTime()>= m_param[m_petternID].aliveTime)
        {
            Delete();
        }

        //  一定距離進んだら拡散開始
        float distance = XMFLOATHelper::XMFLOAT3Distance(m_pos, m_firstPos);
        if (distance >= m_param[m_petternID].startSpreadDistance && !m_isSpread)
        {
            //  中心の弾とそれ以外で処理を分岐
            if (m_num == 0)
            {
                Delete();
            }
            else
            {
                m_unitVec = m_spreadUnitVec;
                m_isSpread = true;
            }
        }

        //  移動
        Move();

        //  中心の弾とそれ以外で処理を分岐
        if (m_num == 0)
        {
            //  モデルの描画
            m_bulletModel.DrawChangePosAndScale(m_pos, m_param[m_petternID].centerBulletSizeMagnification, PMDModelType::UniqueBullet);
        }
        else
        {
            //  モデルの描画
            m_bulletModel.Draw(m_pos, PMDModelType::UniqueBullet);
        }
    }
}

//  発射
void SpreadAttack::Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec,
                         int _num, float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)
{
    //  各パラメータをコピー
    m_petternID = _petternID;   //  パターンIDの設定
    m_pos = _pos;   //  座標
    m_unitVec = _unitVec;   //  発射方向を決定
    m_num = _num;       //  自分の番号
    //  中心の弾以外で処理
    if (m_num != 0)
    {
        m_spreadUnitVec = XMFLOATHelper::XMFLOAT3TiltXZ(m_unitVec, (m_num - 1) * m_param[m_petternID].spreadAngle + m_param[m_petternID].spreadStartAngle); //  拡散方向を決定
        m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, XMFLOATHelper::XMFLOAT3Scale(m_spreadUnitVec, m_param[m_petternID].firstMoveDistace)); //  拡散方向にすこしだけ弾を動かす
    }
    m_firstPos = m_pos;     //  発射時の座標を保存
    m_deltaTime = _deltaTime;   //  デルタタイム
    m_isSpread = false;     //  最初は拡散させていない状態から
    m_isDisplay = true;     //  画面上に登場させる
    m_returnPoolFunc = _returnPoolFunc; //  消去時にこのインスタンス自身をPool内に戻すための関数ポインタ

    //  生存時間計測開始
    m_aliveTimer->Start();
}

//  消去
void SpreadAttack::Delete()
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
void SpreadAttack::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    int num = fileData.GetArraySize(JsonDataType::SpreadAttack, "SpreadAttackData");
    for (int i = 0; i < num; i++)
    {
        Param temp;
        //  外部ファイルのデータを元にパラメータを決定
        temp.aliveTime = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "AliveTime");
        temp.speed = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "Speed");
        temp.centerBulletSizeMagnification = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "CenterBulletSizeMagnification");
        temp.firstMoveDistace = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "FirstMoveDistace");
        temp.startSpreadDistance = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "StartSpreadDistance");
        temp.spreadAngle = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "SpreadAngle") * PI;
        temp.spreadStartAngle = fileData.GetFloatData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "SpreadStartAngle") * PI;
        m_param.emplace_back(temp);
    }

    //  衝突判定用の半径
    m_sphereData->radius = fileData.GetFloatData(JsonDataType::SpreadAttack, "Radius");
}

//  衝突時の処理
void SpreadAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  プレイヤーと衝突したら攻撃を消す
    if (_class.GetObjectTagName() == ObjectTagName::Player)
    {
        Delete();
    }
}

//  移動
void SpreadAttack::Move()
{
    //  各方向への加速度を決定
    float speedX = m_unitVec.x * m_param[m_petternID].speed * m_deltaTime;
    float speedZ = m_unitVec.z * m_param[m_petternID].speed * m_deltaTime;

    //  加速度をもとに座標を決定
    m_pos = XMFLOATHelper::XMFLOAT3Add(m_pos, { speedX,0,speedZ });
}
