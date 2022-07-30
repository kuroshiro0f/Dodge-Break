#include "AppearEnemyDirector.h"

#include <string>

#include "CollisionType.h"
#include "AppearEnemyData.h"
#include "EnemyType.h"
#include "JsonDataType.h"
#include "MapChipData.h"
#include "MapChipType.h"
#include "StageData.h"
#include "Timer.h"
#include "RestartData.h"
#include "EventNotificator.h"
#include "EventType.h"

#include "Singleton.h"
#include "LoadJson.h"
#include "CheckError.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

AppearEnemyDirector::AppearEnemyDirector()
    :m_json(Singleton<LoadJson>::GetInstance())
    , m_appearTimer(new Timer())
    , m_appearWaveNum(0)
    , m_extendCount(0)
    , m_extendTime(0)
    , m_activateEnemyDefeater(nullptr)
    , m_appearEnemy(nullptr)
    , m_appearCount(0)
    , m_enemyData({})
    , m_appearEnemyData({})
#if _DEBUG
    , m_restartTime(0)
    , m_debugCheckPointIndex(0)
#endif
{
}

AppearEnemyDirector::~AppearEnemyDirector()
{
    //  メモリの解放
    for (auto itr : m_enemyData)
    {
        delete itr;
    }
    m_enemyData.clear();
    delete m_appearTimer;

#if _DEBUG
    m_debugCheckPoint.clear();
    m_debugCheckPoint.shrink_to_fit();
#endif
}

//  初期化
void AppearEnemyDirector::Init(const std::function <void(const std::vector<struct AppearEnemyData*>&)> _appearEnemy,
    const std::function<void()> _activateEnemyDefeater, const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  エネミーを登場させるための関数のポインタを取得
    m_appearEnemy = _appearEnemy;
    //  塔を起動するための関数のポインタを取得
    m_activateEnemyDefeater = _activateEnemyDefeater;

    //  データをロード
    Load();

    //  プレイヤーの被弾時にエネミーの出現時間を延長するように関数を登録
    _eventClass->Register(EventType::DamagePlayer, std::bind(&AppearEnemyDirector::ExtendAppearTime, this));

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&AppearEnemyDirector::ReLoad, this));
#endif
}

//  リセット
void AppearEnemyDirector::Reset()
{
    //  最初から出現させる
    m_appearCount = 0;
    m_appearWaveNum = 0;
    //  時間計測クラスのリセット
    m_appearTimer->Reset();
    //  出現予定エネミーのリセット
    m_appearEnemyData.clear();
    //  延長回数をリセット
    m_extendCount = 0;
    //  延長時間をリセット
    m_extendTime = 0;
#if _DEBUG
    //  再開時間のリセット
    m_restartTime = 0;
    //  デバッグ時のリスタート場所は最初からカウントしなおし
    m_debugCheckPointIndex = 0;
#endif
}

//  出現させるための処理を開始する
void AppearEnemyDirector::Start(int _groupNum)
{
    //  出現時間計測の開始
    m_appearTimer->Start();
    //  ウェーブを指定
    m_appearWaveNum = _groupNum;
#if _DEBUG
    //  NOTE: デバッグにおける操作にて、敵の出現順番を動かした場合に行う処理
    //  敵の出現をウェーブに合わせる
    while (m_enemyData[m_appearCount]->waveNum != m_appearWaveNum)
    {
        if (m_appearWaveNum < m_enemyData[m_appearCount]->waveNum)
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            error.CreateErrorMessage("出現させられるエネミーが存在しません。");
        }
        m_appearCount++;
    }
#endif
    //  延長回数をリセット
    m_extendCount = 0;
    //  延長時間をリセット
    m_extendTime = 0;
}

//  更新
void AppearEnemyDirector::Update()
{
    //  出現させるエネミーを決定
    DecideAppearEnemy();
    //  出現させるエネミーがいるなら出現処理
    if (m_appearEnemyData.size() > 0)
    {
        m_appearEnemy(m_appearEnemyData);
        m_appearEnemyData.clear();
    }
}

//  出現時間の延長
void AppearEnemyDirector::ExtendAppearTime()
{
    m_extendCount++;
    m_extendTime = m_extendCount * (RestartData::RETURN_TIME + RestartData::RESTART_TIME);
}

#if _DEBUG
//  指定された時間から再開
void AppearEnemyDirector::RestartCheckPoint(const int _num)
{
    //  出現済エネミーのデータを削除
    m_appearEnemyData.clear();
    //  リスタート地点を決定
    m_debugCheckPointIndex += _num;
    //  添え字が配列の要素数を超えた時の処理
    if (m_debugCheckPointIndex >= m_json.GetArraySize(JsonDataType::Enemy, "DebugCheckPointData"))
    {
        m_debugCheckPointIndex = 0;
        m_appearWaveNum++;
        if (m_appearWaveNum >= m_json.GetArraySize(JsonDataType::Enemy, "EnemyData"))
        {
            m_appearWaveNum = 0;
        }
    }
    else if (m_debugCheckPointIndex < 0)
    {
        m_debugCheckPointIndex = m_json.GetArraySize(JsonDataType::Enemy, "DebugCheckPointData") - 1;
        m_appearWaveNum--;
        if (m_appearWaveNum < 0)
        {
            m_appearWaveNum = m_appearWaveNum >= m_json.GetArraySize(JsonDataType::Enemy, "EnemyData") - 1;
        }
    }
    //  再開時に指定する時間を保存
    m_restartTime = m_debugCheckPoint[m_debugCheckPointIndex];
    
    //  指定した時間以降で最初に登場するエネミーを示す
    //  配列での添え字を取得
    int count = 0;
    for (auto itr : m_enemyData)
    {
        if (itr->appearTime >= m_restartTime && itr->waveNum == m_appearWaveNum)
        {
            m_appearCount = count;
            break;
        }
        count++;
    }

    //  エネミー出現時間管理用のタイマーをリセットしてスタート
    m_appearTimer->Start();
}
#endif

#if _DEBUG
//  再読み込み
void AppearEnemyDirector::ReLoad()
{
    int size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData");
    for (int i = 0; i < size; i++)
    {
        //  エネミーの配置を設定
        *m_enemyData[i] = SetUpEnemyData(i);
    }
}
#endif

//  ロード
void AppearEnemyDirector::Load()
{
    //  エネミーのデータを設定
    int size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData");
    for (int i = 0; i < size; i++)
    {
        m_enemyData.emplace_back(new AppearEnemyData(SetUpEnemyData(i)));
    }

#if _DEBUG
    //  リスタート用のチェックポイントを設定
    size = m_json.GetArraySize(JsonDataType::Enemy, "DebugCheckPointData");
    for (int i = 0; i < size; i++)
    {
        m_debugCheckPoint.emplace_back(m_json.GetFloatData(JsonDataType::Enemy, "DebugCheckPointData", i));
    }
#endif
}

//  出現させるエネミーを決定する
//  NOTE: 外部ファイルにて出現時間順の昇順でソートしてあるため、
//        順番に出現させるだけでいい
void AppearEnemyDirector::DecideAppearEnemy()
{
    //  全てのエネミーが出現済みなら処理をスキップ
    if (m_appearCount >= m_enemyData.size())
    {
        return;
    }
#if _DEBUG
    if (m_appearTimer->GetElapseTime() + m_restartTime >= m_enemyData[m_appearCount]->appearTime + m_extendTime && m_enemyData[m_appearCount]->waveNum == m_appearWaveNum)
    {
        auto a = m_appearTimer->GetElapseTime();
        m_appearEnemyData.emplace_back(m_enemyData[m_appearCount++]);
        //  出現するエネミーがなくなるまで繰り返す
        DecideAppearEnemy();
    }
#else
    if (m_appearTimer->GetElapseTime() >= m_enemyData[m_appearCount]->appearTime + m_extendTime && m_enemyData[m_appearCount]->waveNum == m_appearWaveNum)
    {
        auto a = m_appearTimer->GetElapseTime();
        m_appearEnemyData.emplace_back(m_enemyData[m_appearCount++]);
        //  出現するエネミーがなくなるまで繰り返す
        DecideAppearEnemy();
    }
#endif
    //  塔を起動させる
    m_activateEnemyDefeater();
}

//  出現させるエネミーのデータを設定する
AppearEnemyData AppearEnemyDirector::SetUpEnemyData(int _enemyNum)
{
    //  設定した配置のデータを格納する構造体の変数
    AppearEnemyData retData;

    //  種類を設定
    std::string typeString = m_json.GetStringData(JsonDataType::Enemy, "EnemyData", _enemyNum, "Type");
    if (typeString == "UniqueShootEnemy")
    {
        retData.enemyType = EnemyType::UniqueShootEnemy;
    }
    else if (typeString == "NoAimingShootEnemy")
    {
        retData.enemyType = EnemyType::NoAimingShootEnemy;
    }
    else if (typeString == "AimingShootEnemy")
    {
        retData.enemyType = EnemyType::AimingShootEnemy;
    }

    //  攻撃の種類を設定
    std::string attackTypeString = m_json.GetStringData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AttackType");
    if (attackTypeString == "NoAttack")
    {
        retData.enemyAttackType = EnemyAttackType::NoAttack;
    }
    else if (attackTypeString == "Straight")
    {
        retData.enemyAttackType = EnemyAttackType::Straight;
    }
    else if (attackTypeString == "Fannel")
    {
        retData.enemyAttackType = EnemyAttackType::Fannel;
    }
    else if (attackTypeString == "Zigzag")
    {
        retData.enemyAttackType = EnemyAttackType::Zigzag;
    }
    else if (attackTypeString == "Spread")
    {
        retData.enemyAttackType = EnemyAttackType::Spread;
    }
    else if (attackTypeString == "Line")
    {
        retData.enemyAttackType = EnemyAttackType::Line;
    }

    //  攻撃のパターンIDを設定
    retData.attackPetternID = m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AttackPetternID");

    //  攻撃回数
    retData.attackNum = m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AttackNum");
    //  退場までの時間
    retData.exitTime = m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "ExitTime");

    //  バースト撃ちの設定
    retData.burstTime = m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "BurstTime");
    retData.burstNum = m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "BurstNum");

    //  出現座標と移動先を設定
    int size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "MovePoint");
    for (int i = 0; i < size; i++)
    {
        retData.movePoint.emplace_back(m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "MovePoint", i));
    }
    size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "ControlPoint");
    //  移動時の制御点を設定
    for (int i = 0; i < size; i++)
    {
        retData.controlPoint.emplace_back(m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "ControlPoint", i));
    }
    size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "MoveRate");
    //  移動時の移動速度を設定
    for (int i = 0; i < size; i++)
    {
        retData.moveRate.emplace_back(m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "MoveRate", i));
    }
    //  移動回数を設定
    retData.moveNum = retData.movePoint.size();

    //  登場時の出現座標と移動先を設定
    size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearMovePoint");
    for (int i = 0; i < size; i++)
    {
        retData.appearMovePoint.emplace_back(m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearMovePoint", i));
    }
    size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearControlPoint");
    //  登場時の移動時の制御点を設定
    for (int i = 0; i < size; i++)
    {
        retData.appearControlPoint.emplace_back(m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearControlPoint", i));
    }
    size = m_json.GetArraySize(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearMoveRate");
    //  登場時の移動時の移動速度を設定
    for (int i = 0; i < size; i++)
    {
        retData.appearMoveRate.emplace_back(m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearMoveRate", i));
    }
    //  登場時の移動回数を設定
    retData.appearMoveNum = retData.appearMovePoint.size();

    //  攻撃の発射間隔を設定
    retData.shootInterval = m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "ShootInterval");

    //  出現ウェーブ番号を設定
    retData.waveNum = m_json.GetIntData(JsonDataType::Enemy, "EnemyData", _enemyNum, "WaveNum");

    //  出現時間を設定
    retData.appearTime = m_json.GetFloatData(JsonDataType::Enemy, "EnemyData", _enemyNum, "AppearTime");

    return retData;
}