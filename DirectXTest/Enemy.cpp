#include "Enemy.h"

#include "CollisionType.h"
#include "EnemyType.h"
#include "EnemyData.h"
#include "EnemyState.h"
#include "EnemyAttackType.h"
#include "EnemyAttackDirector.h"
#include "XMFLOATHelper.h"
#include "AppearEnemyData.h"
#include "EffectType.h"
#include "SoundType.h"
#include "PMDModelType.h"
#include "AppearEnemyData.h"
#include "PMDRenderer.h"
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

Enemy::Enemy()
    :m_beforePos({ 0,0,0 })
    , m_deltaTime(0)
    , m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_sound(Singleton<Sound>::GetInstance())
    , m_model(Singleton<PMDModel>::GetInstance())
    , m_shootTimer(new Timer())
    , m_attackDirector(new EnemyAttackDirector())
    , m_param(new AppearEnemyData())
    , m_exitTimer(new Timer())
    , m_burstTimer(new Timer())
    , m_state(EnemyState::StandBy)
    , m_increaseDefeatEnemyNumFunc(nullptr)
    , m_eventClass(nullptr)
    , m_moveRate(0)
    , m_angle(0)
    , m_paramIndex(0)
    , m_nextParamIndex(1)
    , m_isShootFirstAttack(false)
    , m_restartShootTimeAdd(0)
    , m_attackedNum(0)
    , m_burstTargetPos({ 0,0,0 })
    , m_centerPos({ 0,0,0 })
    , m_movePosData({})
{
}

Enemy::~Enemy()
{
    //  メモリの解放
    delete m_shootTimer;
    delete m_exitTimer;
    delete m_burstTimer;
    delete m_attackDirector;
    delete m_param;
}

//  衝突した際の処理
void Enemy::OnCollisionEnter(const CollisionObject& _class)
{
    //  処理なし
}

//  初期化
void Enemy::Init(const std::shared_ptr<class EventNotificator> _eventClass, std::function<void()> _increaseDefeatEnemyNum)
{
    //  衝突判定に登録
    CollisionObject::Init(CollisionType::Enemy);

    //  エネミーの撃破数を増加させる関数へのポインタを取得
    m_increaseDefeatEnemyNumFunc = _increaseDefeatEnemyNum;
    //  イベント通知クラスのインスタンスを取得
    m_eventClass = _eventClass;

    //  外部ファイルからデータをロード
    LoadFileData();

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&Enemy::LoadFileData, this));
#endif

    //  プレイヤーが被弾したときに攻撃行動をやめるように関数を登録
    _eventClass->Register(EventType::DamagePlayer, std::bind(&Enemy::StopAttack, this));
    //  再開したときに攻撃行動も再開するように関数を登録
    _eventClass->Register(EventType::Restart, std::bind(&Enemy::RestartAttack, this));

    //  エネミーのロード
    m_model.SetUp(2);

    //  攻撃クラスの初期化
    m_attackDirector->Init();
}

//  値のリセット
void Enemy::Reset()
{
    //  出現させない
    m_isDisplay = false;
    m_state = EnemyState::StandBy;
    //  攻撃を削除
    m_attackDirector->Delete();
    //  移動割合のリセット
    m_moveRate = 0;
    //  攻撃回数のリセット
    m_attackedNum = 0;
    //  発射間隔のタイマーのリセット
    m_shootTimer->Reset();
    m_burstTimer->Reset();
    m_exitTimer->Reset();
    //  時間計測の開始
    m_shootTimer->Start();
}

//  外部ファイルから読み取ったデータをロード
void Enemy::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  固定移動用に指定する座標を決定
    //  NOTE: 値は外部ファイルから読み込む
    for (int i = 0; i < fileData.GetArraySize(JsonDataType::Stage, "MovePosData"); i++)
    {
        m_movePosData[i] = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "MovePosData", i);
    }
    m_centerPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "CenterPos");
    m_radius = EnemyData::RADIUS;
}

//  エネミーの座標の移動を計算する
void Enemy::TrackMove(const XMFLOAT3& _targetPos)
{
    //  エネミーからプレイヤーへの単位ベクトルを取得
    XMFLOAT3 vec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, _targetPos);
    //  移動速度
    float moveSpeed = EnemyData::SPEED;
    //  移動量
    XMFLOAT3 speed;
    speed.x = vec.x * moveSpeed * m_deltaTime;
    speed.z = vec.z * moveSpeed * m_deltaTime;
    //  エネミーの角度の計算
    //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
    if (_targetPos.x >= m_pos.x)
    {
        m_angle = XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, vec);
    }
    else
    {
        m_angle = -XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, vec);
    }

    //  移動量を現在の座標に加算
    m_pos.x += speed.x;
    m_pos.z += speed.z;
}

//  エネミーの座標を指定した移動
void Enemy::NoTrackMove(const XMFLOAT3& _playerPos, const std::vector<int>& _movePoint, const std::vector<int>& _controlPoint, std::vector<float>& _moveRate, const int _moveNum)
{
    //  moveRateに設定された値が1.0f以上なら、目的地に一瞬で移動
    if (_moveRate[m_paramIndex] >= 1.0f)
    {
        m_moveRate = 1.0f;
    }
    //  moveRateに設定された値が0以下なら、その場からもう移動しない
    else if (_moveRate[m_paramIndex] <= 0)
    {
        //  プレイヤーを狙うエネミーは常にプレイヤーの方へ向く
        if (m_param->enemyType != EnemyType::NoAimingShootEnemy)
        {
            CalculateAngle(_playerPos);
        }
        else
        {
            CalculateAngle(m_movePosData[_movePoint[m_paramIndex]]);
        }
        return;
    }
    //  moveRateに設定された値が0より大きく1.0f未満なら、その分だけ移動割合を加算
    else
    {
        m_moveRate += _moveRate[m_paramIndex] * m_deltaTime;
    }

    //  移動先の座標
    //  NOTE: 角度計算のために直接m_posに入れず、
    //        一度別の変数に計算結果を保持してからm_posに渡す。
    XMFLOAT3 nextPos;

    //  直線移動の処理
    if (_controlPoint[m_paramIndex] == -1)
    {
        //  直線移動
        //  NOTE: 移動する予定の総合距離の何%の位置に移動するのかを、
        //        移動割合を元に計算して、座標を決定する。
        nextPos = XMFLOATHelper::XMFLOAT3CreateBezierCurve(m_movePosData[_movePoint[m_paramIndex]], m_movePosData[_movePoint[m_nextParamIndex]], m_moveRate);
    }
    //  曲線移動の処理
    else
    {
        //  曲線移動
        //  NOTE: 移動する予定の総合距離の何%の位置に移動するのかを、
        //        移動割合を元に計算して、座標を決定する。
        nextPos = XMFLOATHelper::XMFLOAT3CreateBezierCurve(m_movePosData[_movePoint[m_paramIndex]], m_movePosData[_movePoint[m_nextParamIndex]], m_moveRate, m_movePosData[_controlPoint[m_paramIndex]]);
    }

    //  目的地にたどり着くか通り過ぎたら
    if (m_moveRate >= 1.0f)
    {
        //  目的地に到着させる
        nextPos = m_movePosData[_movePoint[m_nextParamIndex]];

        //  移動割合を0に
        m_moveRate = 0;

        //  配列の添え字に設定する番号が、
        //  配列の最後の要素を指しているかチェック
        if (m_nextParamIndex == _moveNum - 1)
        {
            //  登場時の処理中ならstateをAttackに変更
            if (m_state == EnemyState::Appear)
            {
                m_state = EnemyState::Attack;
                //  時間計測の開始
                m_shootTimer->Start();
                m_paramIndex = 0;
                m_nextParamIndex = 1;
            }
            else
            {
                //  次の番号が配列の要素数を超えるなら、0に設定
                m_paramIndex = m_nextParamIndex;
                m_nextParamIndex = 0;
            }
        }
        else
        {
            //  配列の添え字に設定する番号を1つ進める
            m_paramIndex = m_nextParamIndex;
            m_nextParamIndex++;
        }
    }

    //  エネミーを少し浮かせる
    nextPos.y = EnemyData::DISTANCE_FROM_GROUND;

    //  プレイヤーを狙うエネミーは登場時以外プレイヤーの方へ向く
    if (m_param->enemyType != EnemyType::NoAimingShootEnemy && m_state != EnemyState::Appear)
    {
        CalculateAngle(_playerPos);
    }
    else
    {
        CalculateAngle(nextPos);
    }

    //  座標を決定
    m_pos = nextPos;
}

//  退場時の移動
void Enemy::ExitMove()
{
    //  エネミーの移動方向決定
    XMFLOAT3 vec;
    if (m_pos.x == m_centerPos.x && m_pos.z == m_centerPos.z)
    {
        vec = EnemyData::EXIT_VEC_FOR_CENTER_ENEMY;
    }
    else
    {
        vec = XMFLOATHelper::XMFLOAT3Normalize({ m_centerPos.x,EnemyData::DISTANCE_FROM_GROUND,m_centerPos.z }, m_pos);
    }
    //  座標移動
    m_pos.x += vec.x + EnemyData::EXIT_SPEED * m_deltaTime;
    m_pos.z += vec.z + EnemyData::EXIT_SPEED * m_deltaTime;
    //  エネミーの角度の計算
    if (m_centerPos.x <= m_pos.x)
    {
        m_angle = XMFLOATHelper::XMFLOAT3Angle(EnemyData::CRITERION_ANGLE, vec);
    }
    else
    {
        m_angle = -XMFLOATHelper::XMFLOAT3Angle(EnemyData::CRITERION_ANGLE, vec);
    }
}

//  角度の計算
void Enemy::CalculateAngle(const XMFLOAT3& _targetPos)
{
    //  エネミーからターゲットへの単位ベクトルを取得
    XMFLOAT3 vec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, _targetPos);
    //  エネミーの角度の計算
    //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
    if (_targetPos.x >= m_pos.x)
    {
        m_angle = XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, vec);
    }
    else
    {
        m_angle = -XMFLOATHelper::XMFLOAT3Angle({ 0,0,1.0f }, vec);
    }
}

//  エネミー同士が重なった際の移動
void Enemy::OnOverlapCollision(const XMFLOAT3& _pos)
{
    //  エネミーから衝突相手への正規化したベクトル
    XMFLOAT3 vec = XMFLOATHelper::XMFLOAT3Normalize(m_pos, _pos);
    //  移動速度
    float returnMoveSpeed = EnemyData::RETURN_MOVE_DISTANCE;
    //  移動量
    XMFLOAT3 speed;
    speed.x = vec.x * returnMoveSpeed * m_deltaTime;
    speed.z = vec.z * returnMoveSpeed * m_deltaTime;

    //  移動量を現在の座標から減算
    m_pos.x -= speed.x;
    m_pos.z -= speed.z;
}

//  エネミーをセットする
void Enemy::Appear(const struct AppearEnemyData& _data)
{
    //  渡されたエネミーのパラメータを変数に保存
    *m_param = _data;

    //  最初の座標を設定
    m_pos = m_movePosData[m_param->movePoint[0]];

    //  パラメータ内の配列は0番目から使用
    m_paramIndex = 0;
    m_nextParamIndex = 1;

    //  バースト撃ちで狙う座標を初期化
    m_burstTargetPos = { 0,0,0 };
    //  移動割合の初期化
    m_moveRate = 0;
    //  角度の初期化
    m_angle = 0;
    //  攻撃回数の初期化
    m_attackedNum = 0;
    //  リスタート時の攻撃開始までの時間をリセット
    m_restartShootTimeAdd = EnemyData::FIRST_SHOOT_TIME_ADD;
    //  出現時の移動が終わったら即攻撃
    m_isShootFirstAttack = false;

    //  退場時間初期化
    m_exitTimer->Stop();
    m_burstTimer->Stop();
    m_shootTimer->Stop();

    //  攻撃中状態へ
    m_state = EnemyState::Appear;
    //  出現させる
    m_isDisplay = true;
}

//  エネミーを倒す
void Enemy::Defeat()
{
    //  出現中の場合のみ処理を行う
    if (m_isDisplay)
    {
        //  エネミーを消す
        m_isDisplay = false;
        //  エネミーの撃破数を増やす
        m_increaseDefeatEnemyNumFunc();
        m_state = EnemyState::StandBy;
        //  撃破音の再生
        m_sound.Play(SoundType::DefeatEnemySE, false, true);
        //  撃破エフェクトの再生
        switch (m_param->enemyType)
        {
        case EnemyType::AimingShootEnemy:
            m_effect.PlayEffect(EffectType::DefeatAimEnemy, m_pos, false);
            break;
        case EnemyType::NoAimingShootEnemy:
            m_effect.PlayEffect(EffectType::DefeatNoAimEnemy, m_pos, false);
            break;
        case EnemyType::UniqueShootEnemy:
            m_effect.PlayEffect(EffectType::DefeatUniqueShootEnemy, m_pos, false);
            break;
        default:
            break;
        }
        //  攻撃を削除
        m_attackDirector->Delete();
    }
}

//  エネミーが退場する
//  NOTE: 撃破ではなくいなくなるだけ
void Enemy::Exit()
{
    if (m_isDisplay)
    {
        m_state = EnemyState::Exit;
        m_isDisplay = false;
    }
}

//  攻撃の停止
void Enemy::StopAttack()
{
    if (m_isDisplay)
    {
        //  再開後の攻撃開始までの時間を決定
        m_restartShootTimeAdd = m_param->shootInterval - m_shootTimer->GetElapseTime() + EnemyData::RESTART_SHOOT_TIME_ADD;
        //  攻撃を止める
        m_shootTimer->Stop();
    }
    //  攻撃を削除
    m_attackDirector->Delete();
}
//  攻撃の再開
void Enemy::RestartAttack()
{
    if (m_isDisplay)
    {
        //  再開後の最初の攻撃はまだしていない
        m_isShootFirstAttack = false;
        //  発射時間の計測を開始
        m_shootTimer->Start();
    }
}

//  エネミーの種類のゲッター
EnemyType Enemy::GetEnemyType() const
{
    return m_param->enemyType;
}
//  エネミーの状態のゲッター
EnemyState Enemy::GetEnemyState() const
{
    return m_state;
}

//  更新
void Enemy::Update(const XMFLOAT3& _playerPos, const float _deltaTime)
{
    //  デルタタイムの更新
    m_deltaTime = _deltaTime;
    
    //  出現中の場合のみ処理を行う
    if (m_isDisplay)
    {
        //  登場時の移動
        if (m_state == EnemyState::Appear)
        {
            NoTrackMove(_playerPos, m_param->appearMovePoint, m_param->appearControlPoint, m_param->appearMoveRate, m_param->appearMoveNum);
        }
        //  攻撃中の処理
        else if (m_state == EnemyState::Attack)
        {
            //  直前の座標を更新
            m_beforePos = m_pos;
            if (m_param->movePoint.empty())
            {
                //  移動
                TrackMove(_playerPos);
            }
            else if (!m_param->moveRate.empty())
            {
                NoTrackMove(_playerPos, m_param->movePoint, m_param->controlPoint, m_param->moveRate, m_param->moveNum);
            }

            //  最初の攻撃は即発射
            if (!m_isShootFirstAttack)
            {
                //  再開時は、一定時間後に発射
                if (m_shootTimer->GetElapseTime() >= m_restartShootTimeAdd)
                {
                    //  バースト撃ちの処理
                    if (m_param->burstNum > 0)
                    {
                        if (m_attackedNum == 0)
                        {
                            m_burstTargetPos = _playerPos;
                        }
                        //  攻撃を行う
                        m_attackDirector->Shoot(m_param->enemyAttackType, m_param->attackPetternID, m_pos, m_burstTargetPos, _deltaTime);
                    }
                    else
                    {
                        //  攻撃を行う
                        m_attackDirector->Shoot(m_param->enemyAttackType, m_param->attackPetternID, m_pos, _playerPos, _deltaTime);
                    }
                    //  攻撃音の再生
                    m_sound.Play(SoundType::AttackSE, false, true);
                    //  リスタート時の攻撃開始までの時間をリセット
                    m_restartShootTimeAdd = 0;
                    //  攻撃回数加算
                    m_attackedNum++;
                    //  最初の攻撃を行った
                    m_isShootFirstAttack = true;
                    //  時間計測の開始
                    m_shootTimer->Start();
                }
            }
            //  発射可能時間になったら
            else if (m_shootTimer->GetElapseTime() >= m_param->shootInterval && m_param->enemyAttackType != EnemyAttackType::NoAttack
                && (m_attackedNum < m_param->attackNum || m_param->attackNum <= 0))
            {
                //  バースト撃ちの処理
                if (m_param->burstNum > 0)
                {
                    if (m_attackedNum % m_param->burstNum == 0)
                    {
                        m_burstTargetPos = _playerPos;
                    }
                    //  攻撃を行う
                    m_attackDirector->Shoot(m_param->enemyAttackType, m_param->attackPetternID, m_pos, m_burstTargetPos, _deltaTime);
                }
                else
                {
                    //  攻撃を行う
                    m_attackDirector->Shoot(m_param->enemyAttackType, m_param->attackPetternID, m_pos, _playerPos, _deltaTime);
                }
                //  攻撃音の再生
                m_sound.Play(SoundType::AttackSE, false, true);
                //  攻撃回数加算
                m_attackedNum++;
                //  時間計測の開始
                m_shootTimer->Start();
                if (m_param->burstNum > 0 && m_attackedNum % m_param->burstNum == 0)
                {
                    m_shootTimer->Stop();
                    //  バースト撃ちの発射間隔計測開始
                    m_burstTimer->Start();
                }
            }
        }
    }
    //  退場用の移動
    else if (m_state == EnemyState::Exit)
    {
        ExitMove();
        //  エネミーが画面外に消えたら消す
        if (XMFLOATHelper::XMFLOAT3Distance(m_pos, m_centerPos) >= EnemyData::EXIT_DISTANCE_FROM_CENTER)
        {
            m_state = EnemyState::StandBy;
        }
    }

    //  攻撃が画面上にある場合
    if (m_attackDirector->IsDisplay())
    {
        //  攻撃の更新
        m_attackDirector->Update(_playerPos, m_deltaTime);
    }

    //  攻撃回数が指定回数を超えたら退場
    if (m_attackedNum >= m_param->attackNum && m_param->attackNum > 0)
    {
        if (!m_exitTimer->IsStart())
        {
            m_exitTimer->Start();
        }
        else if (m_exitTimer->GetElapseTime() >= m_param->exitTime)
        {
            Exit();
        }
    }

    //  バースト撃ちの更新
    if (m_burstTimer->IsStart())
    {
        if (m_burstTimer->GetElapseTime() >= m_param->burstTime)
        {
            m_burstTargetPos = { 0,0,0 };
            m_shootTimer->Start();
            m_burstTimer->Stop();
        }
    }
}

//  描画
void Enemy::Draw()
{
    //  出現中のエネミーのみ描画を行う
    if (m_isDisplay || m_state == EnemyState::Exit)
    {
        //  エネミーの種類によって処理を分岐
        //  モデルの描画
        switch (m_param->enemyType)
        {
        case EnemyType::AimingShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::AimEnemy);
            break;
        case EnemyType::NoAimingShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::NoAimEnemy);
            break;
        case EnemyType::UniqueShootEnemy:
            m_model.Draw(m_pos, m_angle, PMDModelType::UniqueShootEnemy);
            break;
        default:
            break;
        }

        m_model.Draw({ m_pos.x,EnemyData::SHADOW_POS_Y,m_pos.z }, m_angle, PMDModelType::EnemyShadow);
    }
}