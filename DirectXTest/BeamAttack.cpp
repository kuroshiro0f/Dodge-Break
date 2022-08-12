#include "BeamAttack.h"

#include "CollisionType.h"
#include "ObjectTagName.h"
#include "EffectType.h"
#include "Timer.h"
#include "JsonDataType.h"

#include "Singleton.h"
#include "CollisionManager.h"
#include "EffekseerManager.h"
#include "LoadJson.h"

//  値を半分にするための定数
const float CHANGE_NUM_HALF = 0.5f;

BeamAttack::BeamAttack()
    :m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_deltaTime(0)
    , m_effectHandle(0)
    , m_effectAngle({ 0,0,0 })
    , m_returnPoolFunc(nullptr)
    , m_changeStateTimer(nullptr)
    , m_aliveTimer(nullptr)
    , m_shootPos({ 0,0,0 })
    , m_angle(0)
    , m_length(0)
    , m_param({})
    , m_paramList({})
    , m_state(State::None)
{
    //  処理なし
}

BeamAttack::~BeamAttack()
{
    m_paramList.clear();
    m_paramList.shrink_to_fit();
    delete m_changeStateTimer;
    delete m_aliveTimer;
}

//  初期化
void BeamAttack::Init()
{
    //  衝突判定を行うための初期化処理
    CollisionObject::Init(CollisionType::Box, ObjectTagName::BeamAttack);

    //  外部ファイルのデータを読み込み
    LoadFileData();

    //  箱型オブジェクトのデータを生成
    m_boxData = new BoxData();

    //  各タイマークラスのインスタンスを生成
    m_changeStateTimer = new Timer();
    m_aliveTimer = new Timer();
}

//  リセット
void BeamAttack::Reset()
{
    //  座標をリセット
    m_shootPos = { 0,0,0 };
    m_pos = { 0,0,0 };
    //  角度をリセット
    m_angle = 0;
    //  長さをリセット
    m_length = 0;

    //  タイマーをリセット
    m_changeStateTimer->Reset();

    //  未発射状態に
    m_state = State::NoShoot;

    //  衝突判定を無効に
    m_isDisplay = false;
}

//  更新
void BeamAttack::Update(const float _deltaTime)
{
    //  未発射状態なら更新処理しない
    if (m_state != State::NoShoot)
    {
        //  デルタタイム更新
        m_deltaTime = _deltaTime;

        //  エネルギーを溜める演出の再生中且つ一定時間経過したら、発射開始し衝突判定を有効にする
        if (m_state == State::Charge && m_changeStateTimer->GetElapseTime() >= m_param.chargeTime)
        {
            m_changeStateTimer->Stop();
            m_isDisplay = true;     //  衝突判定を有効に
            m_state = State::Attack;    //  攻撃中状態に変更
        }
        //  攻撃中の場合はビームを伸ばして座標を更新
        if (m_state == State::Attack)
        {
            //  ビームを伸ばす
            Extend();
            //  衝突判定用の角度の更新
            m_boxData->angle = m_angle;

            //  生存時間が終了したら、ビームを消す
            if (m_aliveTimer->GetElapseTime() >= m_param.aliveTime)
            {
                Delete();
            }
        }

        //  エフェクトの更新
        m_effectHandle = m_effect.UpdateLoopPlayEffect(m_shootPos, m_effectHandle, m_param.playSpeed * m_deltaTime, m_effectAngle);
    }
}

//  発射
void BeamAttack::Shoot(int _attackPetternID, const XMFLOAT3& _shootPos, std::function<void(class BeamAttack*)> _returnPoolFunc)
{
    //  今回発射するビームのパラメータを設定
    m_param = m_paramList[_attackPetternID];

    //  発射位置を決定
    m_shootPos = _shootPos;
    m_pos = m_shootPos;
    //  発射角度を設定
    m_angle = m_param.shootAngle * PI;
    //  発射時のビームの長さを設定
    //  NOTE: 発射時なので0
    m_length = 0;

    //  自身のインスタンスをオブジェクトプールに返す関数ポインタを設定
    m_returnPoolFunc = _returnPoolFunc;

    //  衝突判定用のデータを設定
    m_boxData->angle = m_angle;
    m_boxData->width = m_param.thickness;
    m_boxData->height = 0;  //  今回は矩形扱いで衝突判定を行うため、高さは0
    m_boxData->depth = 0;   //  まだ発射していないため0

    //  エフェクト更新用の角度を決定
    m_effectAngle = { 0,-m_angle,0 };

    //  エフェクトの再生開始
    m_state = State::Charge;
    m_effectHandle = m_effect.PlayEffect(EffectType::BeamAttack, m_shootPos, false, m_effectAngle);

    //  状態更新用のタイマーの計測開始
    m_changeStateTimer->Reset();
    m_changeStateTimer->Start();

    //  生存時間用のタイマーの計測開始
    m_aliveTimer->Reset();
    m_aliveTimer->Start();
}

//  攻撃を画面上から削除し、インスタンスをオブジェクトプールに戻す
void BeamAttack::Delete()
{
    //  未発射状態なら削除
    if (m_state != State::NoShoot)
    {
        Reset();        //  各値をリセット
        m_effect.StopEffect(m_effectHandle);    //  エフェクトを停止
        m_returnPoolFunc(this); //  自身のインスタンスをオブジェクトプールに戻す
    }
}

//  衝突時の処理
void BeamAttack::OnCollisionEnter(const CollisionObject& _class)
{
    //  処理なし
    //  NOTE: 現状ビーム攻撃は他のオブジェクトと衝突してもリアクションはしない
}

//  外部ファイルの読み込み
void BeamAttack::LoadFileData()
{
    //  jsonファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  データを読み取って保存
    int size = fileData.GetArraySize(JsonDataType::BeamAttack, "BeamAttackData");
    for (int i = 0; i < size; i++)
    {
        Param tempParam;
        tempParam.aliveTime = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "AliveTime");
        tempParam.playSpeed = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "PlaySpeed");
        tempParam.chargeTime = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "ChargeTime");
        tempParam.shootAngle = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "ShootAngle");
        tempParam.speed = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "Speed");
        tempParam.lengthMax = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "LengthMax");
        tempParam.thickness = fileData.GetFloatData(JsonDataType::BeamAttack, "BeamAttackData", i, "Thickness");

        //  配列に格納
        //  NOTE: AttackPetternIDを添え字に設定して使用
        m_paramList.emplace_back(tempParam);
    }
}

//  ビームを伸ばす
void BeamAttack::Extend()
{
    //  ビームの長さが最長に達していない場合は伸ばす
    float lengthMax = m_param.lengthMax;
    if (m_length < lengthMax)
    {
        //  スピードが0なら即着弾するように
        if (m_param.speed == 0)
        {
            m_length = lengthMax;
        }
        //  スピードが0より大きい場合は計算処理をする
        else
        {
            m_length += m_param.speed * m_deltaTime;
            if (m_length >= lengthMax)
            {
                m_length = lengthMax;
            }
        }

        //  ビームの長さをもとに衝突判定の値を更新する
        //  衝突判定用の奥行を更新
        m_boxData->depth = m_length;
    }
}
