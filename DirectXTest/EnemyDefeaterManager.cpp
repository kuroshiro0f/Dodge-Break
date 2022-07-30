#include "EnemyDefeaterManager.h"

#include <string>

#include "EnemyType.h"
#include "EnemyDefeater.h"
#include "EventNotificator.h"
#include "EventType.h"

#include "Singleton.h"
#include "CheckError.h"
#include "LoadJson.h"

EnemyDefeaterManager::EnemyDefeaterManager()
    :m_error(Singleton<CheckError>::GetInstance())
    , m_enemyDefeater({})
{
}

EnemyDefeaterManager::~EnemyDefeaterManager()
{
    //  メモリの解放
    for (auto itr : m_enemyDefeater)
    {
        delete itr;
    }
    m_enemyDefeater.clear();
    m_enemyDefeater.shrink_to_fit();
}

//  初期化
void EnemyDefeaterManager::Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass,
    const std::function <void(const EnemyType&)> _defeatFunc, const std::function<bool(const EnemyType)> _isDisplayEnemy)
{
    //  エネミーが登場しているか確認するための関数のポインタを取得
    m_isDisplayEnemy = _isDisplayEnemy;

    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  塔をロード
    int num = fileData.GetIntData(JsonDataType::EnemyDefeater, "TOTAL_NUM");
    for (int i = 0; i < num; i++)
    {
        //  一時的な変数にてクラスのインスタンスを生成し、初期化とエラーチェックを行う
        auto enemyDefeater = new EnemyDefeater();
        enemyDefeater->Init(_eventClass, _enemyDefeaterMapperClass, _defeatFunc, i);
        //  エラーチェック
        if (enemyDefeater == nullptr)
        {
            std::string str = "塔のロード中にエラーが発生しました。";
            m_error.CreateErrorMessage(str);
        }
        //  生成したインスタンスを配列に保存
        m_enemyDefeater.emplace_back(enemyDefeater);
    }
    //  イベント通知クラスに関数を登録
    _eventClass->Register(EventType::EnergyMax, std::bind(&EnemyDefeaterManager::ActivateEnemyDefeater, this));
}

//  リセット
void EnemyDefeaterManager::Reset()
{
    //  全ての塔のリセット関数を実行
    for (auto itr : m_enemyDefeater)
    {
        itr->Reset();
    }
}

//  更新
void EnemyDefeaterManager::Update(const float _deltaTime)
{
    //  全ての塔の更新関数を実行
    for (auto itr : m_enemyDefeater)
    {
        itr->Update(_deltaTime);
    }
    //  画面上から対応するエネミーがいなくなった塔の停止を行う
    StopEnemyDefeater();
}

//  描画
void EnemyDefeaterManager::DrawEnemyDefeater()
{
    //  全ての塔の描画関数を実行
    for (auto itr : m_enemyDefeater)
    {
        itr->Draw();
    }
}

//  塔の起動
void EnemyDefeaterManager::ActivateEnemyDefeater()
{
    for (auto itr : m_enemyDefeater)
    {
        itr->Activate(m_isDisplayEnemy(itr->GetEnemyDefeaterType()));
    }
}
//  NOTE: 待機中の塔の種類に対応するエネミーが出現した場合、
//        その塔を起動する
void EnemyDefeaterManager::ActivateStandByEnemyDefeater()
{
    for (auto itr : m_enemyDefeater)
    {
        if (itr->IsStandBy())
        {
            itr->Activate(m_isDisplayEnemy(itr->GetEnemyDefeaterType()));
        }
    }
}

//  塔を停止させる
//  NOTE: 退場して画面からいなくなったエネミーの種類に対応する塔を準備中に変更
void EnemyDefeaterManager::StopEnemyDefeater()
{
    for (auto itr : m_enemyDefeater)
    {
        if (!m_isDisplayEnemy(itr->GetEnemyDefeaterType()) && itr->IsActivate())
        {
            itr->ChangeStandBy();
        }
    }
}
