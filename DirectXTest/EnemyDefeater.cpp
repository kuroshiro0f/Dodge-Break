#include "EnemyDefeater.h"

#include "CollisionType.h"
#include "EnemyType.h"
#include "JsonDataType.h"
#include "EffectType.h"
#include "SoundType.h"
#include "PMDModelType.h"
#include "Timer.h"
#include "EventNotificator.h"
#include "EventType.h"
#include "EnemyDefeaterMapper.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "EffekseerManager.h"
#include "LoadJson.h"
#include "Sound.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

//  塔を構成するモデルの数
static const int MODEL_NUM = 2;

static const float ANGLE_MAX = 2 * PI;

//  衝突からエネミー撃破までの時間
static const float ENEMY_DEFEAT_TIME = 60.0f;

EnemyDefeater::EnemyDefeater()
    :m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_sound(Singleton<Sound>::GetInstance())
    , m_model(Singleton<PMDModel>::GetInstance())
    , m_effectTimer(new Timer())
    , m_defeatTimer(new Timer())
    , m_eventNotificator(nullptr)
    , m_defeatEnemyFunc(nullptr)
    , m_num(0)
    , m_angle(0)
    , m_cubeEffectHandle(0)
    , m_param({})
    , m_type(EnemyType::None)
    , m_state(State::NoActive)
{
}

EnemyDefeater::~EnemyDefeater()
{
    //  メモリの解放
    delete m_effectTimer;
    delete m_defeatTimer;
}

//  初期化
void EnemyDefeater::Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass
    , const std::function <void(const EnemyType&)> _defeatFunc, int _num)
{
    //  衝突判定に登録
    CollisionObject::Init(CollisionType::EnemyDefeater);

    //  イベント通知クラスのインスタンスを取得
    m_eventNotificator = _eventClass;
    //  エネミー撃破関数のポインタを取得
    m_defeatEnemyFunc = _defeatFunc;
    //  番号を登録
    m_num = _num;

    //  塔のモデルをロード
    //  土台と本体の2つ
    m_model.SetUp(MODEL_NUM);

    //  番号によって種類分け
    m_type = static_cast<EnemyType>(_num);

    //  データのロード
    Load();

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&EnemyDefeater::Load, this));
#endif

    //  イベント発生時に呼び出される関数を設定
    m_eventNotificator->Register(EventType::UseEnemyDefeater, std::bind(&EnemyDefeater::Stop, this));
    m_eventNotificator->Register(EventType::HitEnergyEffect, std::bind(&EnemyDefeater::PlayChargeEnergyEffect, this));

    //  EnemyDefeaterMapperに座標を登録
    _enemyDefeaterMapperClass->RegisterEnemyDefeaterPos(m_type, m_pos);
}

//  値のリセット
void EnemyDefeater::Reset()
{
    //  塔を表示
    //  NOTE: ゲーム中に塔が消えることはありませんが、
    //        この変数をtrueにしないと、衝突判定の計算が行われない。
    m_isDisplay = true;

    //  停止した状態から
    m_state = State::NoActive;
    //  エフェクトのリセット
    m_effect.StopEffect(m_cubeEffectHandle);
    m_cubeEffectHandle = 0;
    //  出現させる
    m_isDisplay = true;
    //  時間計測クラスのリセット
    m_effectTimer->Reset();
    m_defeatTimer->Reset();
    //  経過時間の計測の開始
    m_effectTimer->Start();
}

//  データのロード
void EnemyDefeater::Load()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  取得したデータを元に座標を決定
    m_pos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "MovePosData", fileData.GetIntData(JsonDataType::EnemyDefeater, "Pos", m_num));
    //  取得したデータを元にパラメータを決定
    m_radius = fileData.GetFloatData(JsonDataType::EnemyDefeater, "Radius");
    m_param.RotationSpeed = fileData.GetFloatData(JsonDataType::EnemyDefeater, "ROTATION_SPEED");
    m_param.MoveSpeed = fileData.GetFloatData(JsonDataType::EnemyDefeater, "MOVE_SPEED");
    m_param.MoveDistance = fileData.GetFloatData(JsonDataType::EnemyDefeater, "MOVE_DISTANCE");
    m_param.EffectTime = fileData.GetFloatData(JsonDataType::EnemyDefeater, "EFFECT_TIME");
    m_param.EffectPosY = fileData.GetFloatData(JsonDataType::EnemyDefeater, "EFFECT_POS_Y");
    m_param.NecessaryEnergyAmount = fileData.GetFloatData(JsonDataType::Energy, "MaxNum");
}

//  更新
void EnemyDefeater::Update(const float _deltaTime)
{
    //  出現中の場合のみ処理を行う
    if (m_isDisplay)
    {
        //  アニメーションをつける
        if (m_state != State::NoActive)
        {
            //  回転する
            m_angle += m_param.RotationSpeed * _deltaTime;
            if (m_angle >= ANGLE_MAX)
            {
                m_angle = 0;
            }
        }
        //  衝突後に一定時間経過したら敵を倒すためのイベント通知を行う
        if (m_state == State::Hit && m_defeatTimer->GetElapseTime() >= ENEMY_DEFEAT_TIME)
        {
            //  エネミーを倒す
            m_defeatEnemyFunc(m_type);
            m_eventNotificator->Notificate(EventType::DeleteEnemy);
            m_state = State::NoActive;
        }

        //  エフェクトの更新
        m_cubeEffectHandle = m_effect.UpdateLoopPlayEffect(m_pos, m_cubeEffectHandle,_deltaTime);
    }
}

//  描画
void EnemyDefeater::Draw()
{
    //  出現中の場合のみ処理を行う
    if (m_isDisplay)
    {
        //  宙に浮いているブロック部分の描画
        switch (m_type)
        {
        case EnemyType::AimingShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::AimEnemyDefeater);
            break;
        case EnemyType::NoAimingShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::NoAimEnemyDefeater);
            break;
        case EnemyType::UniqueShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::UniqueShootEnemyDefeater);
            break;
        default:
            break;
        }
        //  土台部分の描画
        m_model.Draw({ m_pos.x,0,m_pos.z }, PMDModelType::Pedestal);
    }
}

//  起動
void EnemyDefeater::Activate(bool _isDisplayEnemy)
{
    if (_isDisplayEnemy)
    {
        //  立方体の起動エフェクト再生
        switch (m_type)
        {
        case EnemyType::AimingShootEnemy:
            m_cubeEffectHandle = m_effect.PlayEffect(EffectType::ActivateAimEnemyTower, m_pos, true);
            break;
        case EnemyType::NoAimingShootEnemy:
            m_cubeEffectHandle = m_effect.PlayEffect(EffectType::ActivateNoAimEnemyTower, m_pos, true);
            break;
        case EnemyType::UniqueShootEnemy:
            m_cubeEffectHandle = m_effect.PlayEffect(EffectType::ActivateUniqueShootEnemyTower, m_pos, true);
            break;
        default:
            break;
        }
        //  起動音を再生
        m_sound.Play(SoundType::ActivateEnemyDefeaterSE, false, true);
        //  起動する
        m_state = State::Active;
    }
    else
    {
        //  待機中に
        m_state = State::StandBy;
    }
}

//  停止する
void EnemyDefeater::Stop()
{
    //  エフェクトを停止
    m_effect.StopEffect(m_cubeEffectHandle);
    //  衝突中に変更
    m_state = State::NoActive;
}

//  準備中に変更する
void EnemyDefeater::ChangeStandBy()
{
    m_state = State::StandBy;
    //  エフェクトを停止
    m_effect.StopEffect(m_cubeEffectHandle);
}

//  エネルギーを溜めているエフェクトの再生
void EnemyDefeater::PlayChargeEnergyEffect()
{
    switch (m_type)
    {
    case EnemyType::AimingShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::ChargeAimEnergy, m_pos, false);
        break;
    case EnemyType::NoAimingShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::ChargeNoAimEnergy, m_pos, false);
        break;
    case EnemyType::UniqueShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::ChargeUniqueShootEnergy, m_pos, false);
        break;
    default:
        break;
    }
    //  エネルギーの吸収音を再生
    m_sound.Play(SoundType::AbsorpEnergySE, false, true);
}

//  衝突時の処理
void EnemyDefeater::OnCollisionEnter(const CollisionObject& _class)
{
    //  衝突した相手がプレイヤー且つエネルギーが必要量溜まっていたら
    if (_class.GetType() == CollisionType::Player && m_state == State::Active)
    {
        //  破壊音を再生
        m_sound.Play(SoundType::UseEnemyDefeaterSE, false, true);
        //  エフェクトを停止
        m_effect.StopEffect(m_cubeEffectHandle);
        //  塔が使用されたことを通知
        m_eventNotificator->Notificate(EventType::UseEnemyDefeater);
        //  衝突中に変更
        m_state = State::Hit;
        m_defeatTimer->Start();
        //  衝突処理
        OnCollisionEnterForPlayer();
    }
}

//  プレイヤーとの衝突時の処理
void EnemyDefeater::OnCollisionEnterForPlayer()
{
    //  EnemyTypeによって処理を分岐
    //  指定されたTypeのエネミーを倒す
    //  NOTE: プレイヤー側にエネミーを倒す処理があると、
    //        エネルギーの実装に伴い処理順などを気にする必要が発生し、
    //        且つEnemyDefeaterに敵を倒す処理があるほうが自然だろうと思ったたため、
    //        関数の呼び出す場所をプレイヤークラスからこちらに変更
    switch (m_type)
    {
    case EnemyType::AimingShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::HitAimEnemyTower, m_pos,false);
        break;
    case EnemyType::NoAimingShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::HitNoAimEnemyTower, m_pos,false);
        break;
    case EnemyType::UniqueShootEnemy:
        //  エフェクトを再生
        m_effect.PlayEffect(EffectType::HitUniqueShootEnemyTower, m_pos,false);
        break;
    default:
        break;
    }
}