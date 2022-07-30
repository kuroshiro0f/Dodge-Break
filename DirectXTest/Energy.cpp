#include "Energy.h"

#include <functional>

#include "PMDModelType.h"
#include "EffectType.h"
#include "EventType.h"
#include "EventNotificator.h"

#include "Singleton.h"
#include "LoadJson.h"
#include "PMDModel.h"
#include "EffekseerManager.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

Energy::Energy()
    :m_energy(0)
    , m_maxEnergyNum(0)
    , m_firstEnergy(0)
    , m_changeAmout({})
    , m_eventClass(nullptr)
{
}

Energy::~Energy()
{
    //  処理なし
}

//  初期化
void Energy::Init(std::shared_ptr<class EventNotificator> _eventClass)
{
    //  外部ファイルからデータをロード
    LoadFileData();

    //  イベント通知クラスのインスタンスを取得
    m_eventClass = _eventClass;

    //  イベント発生時に呼び出される関数を設定
    m_eventClass->Register(EventType::HitEnergyEffect, std::bind(&Energy::IncreaseForDodge, this));
    m_eventClass->Register(EventType::DamagePlayer, std::bind(&Energy::IncreaseForDamage, this));
    m_eventClass->Register(EventType::DeleteEnemy, std::bind(&Energy::DecreaseForDeleteEnemy, this));

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
    m_eventClass->Register(EventType::DamagePlayer, std::bind(&Energy::IncreaseForDamage, this));
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&Energy::LoadFileData, this));
#endif
}

//  リセット
void Energy::Reset()
{
    //  エネルギーのリセット
    m_energy = m_firstEnergy;
}

//  外部ファイルから読み取ったデータをロード
void Energy::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  外部ファイルのデータをもとにエネルギーの初期値を決定
    m_firstEnergy = fileData.GetFloatData(JsonDataType::Energy, "FirstEnergy");
    //  外部ファイルのデータをもとにエネルギーの最大量を決定
    m_maxEnergyNum = fileData.GetFloatData(JsonDataType::Energy, "MaxNum");
    //  外部ファイルのデータをもとにエネルギーの変化量を決定
    m_changeAmout.dodge = fileData.GetFloatData(JsonDataType::Energy, "IncreaseForDodge");
    m_changeAmout.damage = fileData.GetFloatData(JsonDataType::Energy, "IncreaseForDamage");
    m_changeAmout.deleteEnemy = fileData.GetFloatData(JsonDataType::Energy, "DecreaseForDeleteEnemy");

    //  エネルギーを初期化
    m_energy = m_firstEnergy;
}

//  回避によるエネルギーの増加
void Energy::IncreaseForDodge()
{
    if (m_energy < m_maxEnergyNum)
    {
        m_energy += m_changeAmout.dodge;
        if (m_energy >= m_maxEnergyNum)
        {
            m_energy = m_maxEnergyNum;
            m_eventClass->Notificate(EventType::EnergyMax);
        }
    }
}

//  被弾によるエネルギーの増加
void Energy::IncreaseForDamage()
{
    if (m_energy < m_maxEnergyNum)
    {
        m_energy += m_changeAmout.damage;
        //  最大値を超えないように調整
        if (m_energy >= m_maxEnergyNum)
        {
            m_energy = m_maxEnergyNum;
            m_eventClass->Notificate(EventType::EnergyMax);
        }
    }
}

//  エネミーの撃破によるエネルギーの減少
void Energy::DecreaseForDeleteEnemy()
{
    if (m_energy == m_maxEnergyNum)
    {
        m_energy += m_changeAmout.deleteEnemy;
        if (m_energy <= 0)
        {
            m_energy = 0;
        }
    }
}