#include "EnemyManager.h"

#include <string>

#include "EnemyData.h"
#include "EnemyType.h"
#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyAttackType.h"
#include "AppearEnemyData.h"
#include "StageData.h"
#include "Timer.h"

#include "Singleton.h"
#include "CheckError.h"

//  出現を止める時間
static const float APPEAR_STOP_TIME = 0;

EnemyManager::EnemyManager()
    :m_error(Singleton<CheckError>::GetInstance())
    , m_mapData(new MapData())
    , m_stopAppearTimer(new Timer())
    , m_defeatEnemyNum(0)
    , m_stopAppearEnemyType(EnemyType::NullType)
    , m_enemy({})
{
}

EnemyManager::~EnemyManager()
{
    //  メモリの解放
    delete m_mapData;
    delete m_stopAppearTimer;
    for (auto itr : m_enemy)
    {
        delete itr;
    }
    m_enemy.clear();
    m_enemy.shrink_to_fit();
}

//  初期化
void EnemyManager::Init(const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  エネミーのロード
    for (int i = 0; i < EnemyData::TOTAL_NUM; i++)
    {
        //  一時的な変数にてクラスのインスタンスを生成し、初期化とエラーチェックを行う
        auto enemy = new Enemy();
        enemy->Init(_eventClass, std::bind(&EnemyManager::IncreaseDefeatEnemyNum, this));
        //  エラーチェック
        if (enemy == nullptr)
        {
            std::string str = "エネミーのロード中にエラーが発生しました。";
            m_error.CreateErrorMessage(str);
        }
        //  生成したインスタンスを配列に保存
        m_enemy.emplace_back(enemy);
    }
}

//  リセット
void EnemyManager::Reset()
{
    //  全てのエネミーのリセット関数を実行
    for (auto itr : m_enemy)
    {
        itr->Reset();
    }
    //  撃破回数のリセット
    m_defeatEnemyNum = 0;
}

//  更新
void EnemyManager::Update(const XMFLOAT3& _playerPos, const float _deltaTime)
{
    //  全てのエネミーの更新関数を実行
    for (auto itr : m_enemy)
    {
        itr->Update(_playerPos, _deltaTime);
    }
}

//  描画
void EnemyManager::DrawEnemy()
{
    //  全てのエネミーの描画関数を実行
    for (auto itr : m_enemy)
    {
        itr->Draw();
    }
}

//  エネミーを出現させる
void EnemyManager::AppearEnemy(const std::vector<struct AppearEnemyData*>& _data)
{
    //  セット個数カウント用
    int count = 0;

    //  生成しているエネミークラスのインスタンスの数だけループ
    auto dataSize = _data.size();
    for (auto itr : m_enemy)
    {
        //  出現済のエネミーはセットしない
        if (itr->IsDisplay())
        {
            continue;
        }

        //  出現をスキップさせているエネミーを飛ばす
        if (m_stopAppearTimer->GetElapseTime() <= APPEAR_STOP_TIME && _data[count]->enemyType == m_stopAppearEnemyType)
        {
            count++;
            itr--;
        }
        else
        {
            //  出現処理を行う
            itr->Appear(*_data[count]);
            count++;
        }

        //  指定数セットしたらループを抜ける
        if (count >= dataSize)
        {
            break;
        }
    }
}

//  エネミーを倒す
void EnemyManager::DefeatEnemy(const EnemyType _type)
{
    //  指定された色と同じ色のエネミーを探して、倒す処理を行う。
    for (auto itr : m_enemy)
    {
        if (itr->GetEnemyType() == _type)
        {
            itr->Defeat();
        }
    }
    m_stopAppearEnemyType = _type;
    m_stopAppearTimer->Start();
}

//  全てのエネミーを退場させる
void EnemyManager::ExitAllEnemy()
{
    for (auto itr : m_enemy)
    {
        itr->Exit();
    }
}

//  エネミーは画面上に存在するか
bool EnemyManager::IsDisplayEnemy()
{
    for (auto itr : m_enemy)
    {
        if (itr->GetEnemyState() != EnemyState::StandBy)
        {
            return true;
        }
    }
    return false;
}

//  指定した種類のエネミーは画面上に存在するか
bool EnemyManager::IsDisplayEnemyType(const EnemyType _type)
{
    for (auto itr : m_enemy)
    {
        if (itr->GetEnemyState() != EnemyState::StandBy && itr->GetEnemyType() == _type)
        {
            return true;
        }
    }
    return false;
}

//  エネミーの撃破数の増加
void EnemyManager::IncreaseDefeatEnemyNum()
{
    m_defeatEnemyNum++;
}