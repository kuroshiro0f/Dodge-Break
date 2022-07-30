#include "PlayerManager.h"

#include "Player.h"
#include "EffectType.h"
#include "StageData.h"
#include "Timer.h"

#include "IPlayerParameterGetter.h"

#include "Singleton.h"
#include "EffekseerManager.h"

//  エフェクトの位置調整用
//  NOTE: プレイヤーの登場エフェクト時に、影がエフェクトからはみ出るため、エフェクトの位置を調整
static const float FIX_EFFECT_POS_Z = -1.8f;

PlayerManager::PlayerManager()
    :m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_player(new Player())
    , m_mapSize({ 0,0,0 })
{
}

PlayerManager::~PlayerManager()
{
    //  メモリの解放
    delete m_player;
}

//  初期化
void PlayerManager::Init(const std::function<void()> _gameOver, const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass)
{
    //  プレイヤーの初期化
    m_player->Init(_gameOver, _eventClass, _enemyDefeaterMapperClass);
}

//  リセット
void PlayerManager::Reset()
{
    //  プレイヤーのリセット
    m_player->Reset();
}

//  更新
void PlayerManager::Update(const float _deltaTime)
{
    m_player->Update(m_mapSize, _deltaTime);
}

//  描画
void PlayerManager::DrawPlayer()
{
    m_player->Draw();
}

//  プレイヤーを出現させる
void PlayerManager::AppearPlayer()
{
    m_player->Appear();
}

//  退場させる
void PlayerManager::ExitPlayer(const float _deltaTime)
{
    m_player->Exit(_deltaTime);
}

//  死んでいるか
bool PlayerManager::IsPlayerDead()const
{
    if (!m_player->IsDisplay())
    {
        return true;
    }
    return false;
}

//  プレイヤーの座標のゲッター
XMFLOAT3 PlayerManager::GetPlayerPos()const
{
    return m_player->GetPos();
}

//  プレイヤーのパラメータを得るためのクラスのインスタンスを返す
IPlayerParameterGetter* PlayerManager::GetPlayerParameterGetter() const
{
    return static_cast<IPlayerParameterGetter*>(m_player);
}

#if _DEBUG
//  ゲームを再開するための処理
//  NOTE: デバッグで特定の時間からゲームを再開する際に、
//        プレイヤーの登場演出などは省いて、
//        座標や残機などのステータスのみを初期化する処理
void PlayerManager::Restart()
{
    m_player->Restart();
}
#endif