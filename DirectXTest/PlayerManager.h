#pragma once

#include <DirectXMath.h>
#include <functional>
#include <memory>

using namespace DirectX;

//  エフェクトの再生間隔
const float EFFECT_TIME = 30.0f;

enum class CollisionColorType;
//  プレイヤーの更新や描画を管理するクラス
class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager();

    //  初期化
    void Init(const std::function<void()> _gameOver, const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass);
    //  リセット
    void Reset();

    //  更新
    void Update(const float _deltaTime);

    //  描画
    void DrawPlayer();

    //  プレイヤーを出現させる
    void AppearPlayer();

    //  プレイヤーを退場させる
    void ExitPlayer(const float _deltaTime);

    //  プレイヤーは死んでいるか
    bool IsPlayerDead()const;
    //  プレイヤーの座標のゲッター
    XMFLOAT3 GetPlayerPos()const;
    //  プレイヤーのパラメータを得るためのクラスのインスタンスを返す
    class IPlayerParameterGetter* GetPlayerParameterGetter()const;

    //  ゲームを再開するための処理
    //  NOTE: デバッグで特定の時間からゲームを再開する際に、
    //        プレイヤーの登場演出などは省いて、
    //        座標や残機などのステータスのみを初期化する処理
    void Restart();
private:
    //  マップのサイズ
    XMFLOAT3 m_mapSize;

    //  クラスへのポインタメンバ変数
    class Player* m_player; //  プレイヤー

    //  シングルトンクラスへの参照
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
};