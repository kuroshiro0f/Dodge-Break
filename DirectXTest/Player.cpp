#include "Player.h"

#include "CollisionType.h"
#include "PlayerData.h"
#include "EnemyType.h"
#include "Energy.h"
#include "DodgeRange.h"
#include "EnergyEffect.h"
#include "XMFLOATHelper.h"
#include "EnumIterator.h"
#include "EffectType.h"
#include "SoundType.h"
#include "PMDModelType.h"
#include "StageData.h"
#include "PMDRenderer.h"
#include "Mouse.h"
#include "Timer.h"
#include "EventNotificator.h"
#include "EventType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "EffekseerManager.h"
#include "Sound.h"
#include "LoadJson.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

//  TODO: デバッグ機能として無敵と回復を行えるようにする

Player::Player()
    :m_deltaTime(0)
    , m_vec({ 0,0,0 })
    , m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_sound(Singleton<Sound>::GetInstance())
    , m_model(Singleton<PMDModel>::GetInstance())
    , m_mouse(new Mouse())
    , m_appearTimer(new Timer())
    , m_invincibleTimer(new Timer())
    , m_invincibleFlashingTimer(new Timer())
    , m_restartTimer(new Timer())
    , m_exitTimer(new Timer())
    , m_mapData(new MapData())
    , m_dodgeRange(new DodgeRange())
    , m_energy(new Energy())
    , m_eventClass(nullptr)
    , m_gameOver(nullptr)
    , m_dodgeEffectHandle(0)
    , m_damageEffectHandle(0)
    , m_exitEffectHandle(0)
    , m_dodgeNum(0)
    , m_angle(0)
    , m_life(0)
    , m_param({})
    , m_state(State::None)
    , m_isDisplayModel(true)
    , m_isStartExit(false)
{
    //  エネルギーのエフェクトを指定するロード
    for (int i = 0; i < PlayerData::ENERGY_EFFECT_NUM; i++)
    {
        m_energyEffect.emplace_back(new EnergyEffect());
    }
}

Player::~Player()
{
    //  メモリの解放
    delete m_mouse;
    delete m_appearTimer;
    delete m_invincibleTimer;
    delete m_invincibleFlashingTimer;
    delete m_restartTimer;
    delete m_dodgeRange;
    delete m_exitTimer;
    delete m_mapData;
    for (auto itr : m_energyEffect)
    {
        delete itr;
    }
    m_energyEffect.clear();
    m_energyEffect.shrink_to_fit();
}

//  初期化
void Player::Init(const std::function <void()> _gameOver, const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass)
{
    //  衝突判定に登録
    CollisionObject::Init(CollisionType::Player);

    //  イベント通知クラスのインスタンスを取得
    m_eventClass = _eventClass;
    //  ゲームオーバー関数のポインタを取得
    m_gameOver = _gameOver;

    //  外部ファイルからデータをロード
    LoadFileData();

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&Player::LoadFileData, this));
#endif

    //  回避時の処理を行う関数をイベント通知クラスにセット
    m_eventClass->Register(EventType::Dodge, std::bind(&Player::Dodge, this));

    //  回避範囲クラスの初期化
    m_dodgeRange->Init(_eventClass);
    //  エネルギークラスの初期化
    m_energy->Init(_eventClass);
    //  エネルギーエフェクトクラスの初期化
    for (auto itr : m_energyEffect)
    {
        itr->Init(_eventClass, _enemyDefeaterMapperClass);
    }

    //  マウスの初期化
    m_mouse->Init();

    //  モデルを描画するための設定
    m_model.SetUp(2);

    //  半径の初期化
    m_radius = m_param.radius;
}

//  値のリセット
void Player::Reset()
{
    //  座標のリセット
    m_pos = m_param.pos;
    //  角度のリセット
    m_angle = 0;
    //  残機を最大値に
    m_life = m_param.maxLife;
    //  出現していない状態から
    m_state = State::NoDisplay;
    //  出現させない
    m_isDisplay = false;

    //  時間計測クラスのリセット
    m_invincibleTimer->Reset();
    m_invincibleFlashingTimer->Reset();
    m_restartTimer->Reset();

    //  エフェクトのリセット
    m_effect.StopEffect(m_dodgeEffectHandle);
    m_effect.StopEffect(m_damageEffectHandle);
    m_effect.StopEffect(m_exitEffectHandle);
    m_dodgeEffectHandle = 0;
    m_damageEffectHandle = 0;
    m_exitEffectHandle = 0;

    //  回避回数カウントのリセット
    m_dodgeNum = 0;

    //  退場開始したか
    m_isStartExit = false;

    //  エネルギーのエフェクトのリセット
    for (auto itr : m_energyEffect)
    {
        itr->Reset();
    }
    //  エネルギーのリセット
    m_energy->Reset();
    //  回避範囲のリセット
    m_dodgeRange->Reset();
}

//  外部ファイルから読み取ったデータをロード
void Player::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  取得したデータを元に初期座標を決定
    m_param.pos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "CenterPos");
    m_param.pos.y += PlayerData::DISTANCE_FROM_GROUND;
    //  取得したデータを元にパラメータを決定
    m_param.maxSpeed = fileData.GetFloatData(JsonDataType::Player, "MaxSpeed");
    m_param.radius = fileData.GetFloatData(JsonDataType::Player, "Radius");
    m_param.actualRadius = fileData.GetFloatData(JsonDataType::Player, "ActualRadius");
    m_param.maxLife = fileData.GetIntData(JsonDataType::Player, "MaxLife");
    m_param.invincibleTime = fileData.GetFloatData(JsonDataType::Player, "InvincibleTime");
}

//  エネルギー量のゲッター
int Player::GetEnergy()const
{
    return m_energy->GetEnergy();
}

#if _DEBUG
//  ゲームを再開するための処理
//  NOTE: デバッグで特定の時間からゲームを再開する際に、
//        プレイヤーの登場演出などは省いて、
//        座標や残機などのステータスのみを初期化する処理
void Player::Restart()
{
    //  座標の初期化
    m_pos = m_param.pos;
    //  角度の初期化
    m_angle = 0;
    //  残機を最大値に
    m_life = m_param.maxLife;
    //  行動中から
    m_state = State::Play;
    //  出現させる
    m_isDisplay = true;
}
#endif

//  衝突した際の処理
void Player::OnCollisionEnter(const CollisionObject& _class)
{
    //  衝突相手のタイプによって処理を分岐
    switch (_class.GetType())
    {
    case CollisionType::Enemy:
        //  残機を減らす
        ReduceLife();
        break;
    case CollisionType::EnemyAttack:
        //  残機を減らす
        ReduceLife();
        //  回避失敗
        m_dodgeRange->DodgeFailed(_class);
        break;
    default:
        break;
    }

    //  残機が尽きたら
    if (m_life < 0)
    {
        //  ゲームオーバー用関数を実行
        m_gameOver();
        //  撃破状態へ
        m_state = State::Defeat;
        //  プレイヤーを消す
        m_isDisplay = false;
    }
}

//  更新
void Player::Update(const XMFLOAT3& _mapSize, const float _deltaTime)
{
    //  デルタタイムを更新
    m_deltaTime = _deltaTime;
    if (m_isDisplay)
    {
        //  マウスの更新
        m_mouse->Update();

        if (m_deltaTime != 0)
        {
            //  移動
            Move();
        }

        //  無敵時間の判定を行う
        UpdateInvincibleTime();

        //  無敵且つ無敵時間の点滅間隔が来たらモデルの表示を変更
        if (m_state == State::Invincible && m_invincibleFlashingTimer->GetElapseTime() >= PlayerData::INVINCIBLE_FLASHING_INTERVAL)
        {
            if (m_isDisplayModel)
            {
                m_isDisplayModel = false;
            }
            else
            {
                m_isDisplayModel = true;
            }
            m_invincibleFlashingTimer->Start();
        }
        else if (m_state != State::Invincible)
        {
            m_isDisplayModel = true;
        }

        //  場外への移動を制限
        StayInsideMap(_mapSize);

        //  無敵中は回避しないようにする
        if (m_state == State::Invincible)
        {
            m_dodgeRange->UpdateInvincible(m_pos);
        }
        else
        {
            //  回避範囲の更新
            m_dodgeRange->Update(m_pos);
        }
    }
    else if (m_restartTimer->GetElapseTime() >= PlayerData::RESTART_TIME && (m_state == State::Damage || m_state == State::Appear) && m_state != State::Defeat)
    {
        //  再出現させる
        Appear();

        //  出現が終了すれば
        if (m_isDisplay)
        {
            //  再開することを通知
            m_eventClass->Notificate(EventType::Restart);
            //  無敵状態になる
            m_state = State::Invincible;
            //  無敵状態の点滅開始
            m_invincibleFlashingTimer->Start();
            //  時間計測の開始
            m_invincibleTimer->Start();
        }
    }
    //  エネルギーのエフェクトの更新
    for (auto itr : m_energyEffect)
    {
        itr->Update(_deltaTime);
    }
    //  エフェクトの更新
    m_damageEffectHandle = m_effect.UpdateLoopPlayEffect(m_pos, m_damageEffectHandle, _deltaTime);
    m_dodgeEffectHandle = m_effect.UpdateLoopPlayEffect(m_pos, m_dodgeEffectHandle, _deltaTime, { 0,m_angle,0 });
}

//  描画
void Player::Draw()
{
    //  出現中の場合のみ処理を行う
    if (m_isDisplay)
    {
        if (m_isDisplayModel)
        {
            //  モデルの描画
            m_model.Draw(m_pos, m_angle, PMDModelType::Player);
        }
        m_model.Draw({ m_pos.x,PlayerData::SHADOW_POS_Y,m_pos.z }, m_angle, PMDModelType::PlayerShadow);
    }
}

//  回避時の処理
void Player::Dodge()
{
    //  現在画面上にいないエネルギーのエフェクトを選んで発射する
    for (auto itr : m_energyEffect)
    {
        if (!itr->IsDisplay())
        {
            itr->Shoot(m_pos, m_energy->IsEnergyMax());
            //  回避音の再生
            m_sound.Play(SoundType::DodgeSE, false, true);
            //  回避回数を増加
            m_dodgeNum++;
            break;
        }
    }
}

//  出現させる
void Player::Appear()
{
    //  非表示か被弾状態なら
    if (m_state == State::NoDisplay || m_state == State::Damage)
    {
        //  出現中に状態を変更
        m_state = State::Appear;
        //  座標の初期化
        m_pos = m_param.pos;
        //  角度の初期化
        m_angle = 0;
        //  出現演出開始
        m_appearTimer->Start();
        //  出現エフェクト再生
        m_effect.PlayEffect(EffectType::AppearancePlayer, { m_pos.x,m_pos.y,m_pos.z + PlayerData::FIX_EFFECT_POS_Z }, false);
        //  出現音の再生
        m_sound.Play(SoundType::AppearPlayerSE, false, true);
    }

    //  出現時間になったらプレイヤーを出現
    if (m_appearTimer->GetElapseTime() >= PlayerData::APPEAR_TIMER)
    {
        m_isDisplay = true;
        m_state = State::Play;
    }
}

//  プレイヤーを退場させる
void Player::Exit(const float _deltaTime)
{
    //  デルタタイムの更新
    m_deltaTime = _deltaTime;

    //  まだ退場開始前なら
    if (!m_isStartExit)
    {
        //  退場時間計測開始
        m_exitTimer->Start();
        //  エフェクト再生開始
        m_exitEffectHandle = m_effect.PlayEffect(EffectType::ExitPlayer, m_pos, false);
        //  退場音の再生
        //  NOTE; 出現音と同じ
        m_sound.Play(SoundType::AppearPlayerSE, false, true);
        //  退場開始
        m_isStartExit = true;
    }

    //  退場エフェクト更新
    m_exitEffectHandle = m_effect.UpdateLoopPlayEffect(m_pos, m_exitEffectHandle, m_deltaTime);

    //  エフェクト再生後一定時間でプレイヤーを消す
    if (m_exitTimer->GetElapseTime() >= PlayerData::EXIT_TIMER)
    {
        m_isDisplay = false;
    }
}

//  移動
void Player::Move()
{
    //  使いまわす変数を格納
    float mousePosX = m_mouse->GetMouseWorldPos().x;
    float actualRadius = m_param.actualRadius;
    float moveSpeed = m_param.maxSpeed;

    //  最高速度を決定
    if (XMFLOATHelper::XMFLOAT3Distance(m_pos, m_mouse->GetMouseWorldPos()) >= moveSpeed + actualRadius)
    {
        //  プレイヤーからマウスへの単位ベクトル
        m_vec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, m_mouse->GetMouseWorldPos());

        //  速度をもとに座標を変更する
        m_pos.x += m_vec.x * moveSpeed * m_deltaTime;
        m_pos.z += m_vec.z * moveSpeed * m_deltaTime;

        //  プレイヤーの角度の計算
        //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
        if (mousePosX >= m_pos.x)
        {
            m_angle = XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, m_vec);
        }
        else
        {
            m_angle = -XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, m_vec);
        }
    }
    //  プレイヤーの先端がマウスポインタの位置に置かれるように移動
    else if (XMFLOATHelper::XMFLOAT3Distance(m_pos, m_mouse->GetMouseWorldPos()) >= actualRadius)
    {
        //  プレイヤーからマウスへの単位ベクトル
        m_vec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, m_mouse->GetMouseWorldPos());

        //  マウスポインタの位置にプレイヤーの先端が位置するように、プレイヤーの座標を調整
        m_pos.x = mousePosX - m_vec.x * actualRadius;
        m_pos.z = m_mouse->GetMouseWorldPos().z - m_vec.z * actualRadius;

        //  プレイヤーの角度の計算
        //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
        if (mousePosX >= m_pos.x)
        {
            m_angle = XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, m_vec);
        }
        else
        {
            m_angle = -XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, m_vec);
        }
    }
}

//  無敵時間の更新
void Player::UpdateInvincibleTime()
{
    //  無敵時間を過ぎていれば、無敵状態を解除
    if (m_invincibleTimer->GetElapseTime() >= m_param.invincibleTime)
    {
        m_state = State::Play;
    }
}

//  残機を減らす
void Player::ReduceLife()
{
    if (m_isDisplay && m_state != State::Invincible)
    {
        //  残機を減らす
        m_life--;

        //  ダメージエフェクトを再生
        m_damageEffectHandle = m_effect.PlayEffect(EffectType::DamagePlayer, m_pos, false);
        //  被弾音の再生
        m_sound.Play(SoundType::DamagePlayerSE, false, true);

        //  一度プレイヤーを消す
        m_isDisplay = false;

        //  状態を被弾に変更
        m_state = State::Damage;

        //  プレイヤーがダメージを受けたことを通知
        m_eventClass->Notificate(EventType::DamagePlayer);

        //  再開までの時間計測開始
        m_restartTimer->Start();
    }
}

//  プレイヤーが場外へ出ないようにする
void Player::StayInsideMap(const XMFLOAT3& _mapSize)
{
    //  座標
    float posX = m_pos.x;
    float posZ = m_pos.z;
    //  初期座標
    float firstPosX = m_param.pos.x;
    float firstPosZ = m_param.pos.z;

    //  場外へ出たら位置を戻す
    if (posX >= firstPosX + PlayerData::MAP_SIZE * PlayerData::HALF_SCALE)
    {
        m_pos.x = firstPosX + PlayerData::MAP_SIZE * PlayerData::HALF_SCALE;
    }
    if (posX <= firstPosX - PlayerData::MAP_SIZE * PlayerData::HALF_SCALE)
    {
        m_pos.x = firstPosX - PlayerData::MAP_SIZE * PlayerData::HALF_SCALE;
    }
    if (posZ >= firstPosZ + PlayerData::MAP_SIZE * PlayerData::HALF_SCALE)
    {
        m_pos.z = firstPosZ + PlayerData::MAP_SIZE * PlayerData::HALF_SCALE;
    }
    if (posZ <= firstPosZ - PlayerData::MAP_SIZE * PlayerData::HALF_SCALE)
    {
        m_pos.z = firstPosZ - PlayerData::MAP_SIZE * PlayerData::HALF_SCALE;
    }
}