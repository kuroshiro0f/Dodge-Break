#pragma once
#include "CollisionObject.h"
#include "IPlayerParameterGetter.h"

#include <DirectXMath.h>
#include <functional>
#include <memory>
#include <vector>

using namespace DirectX;

//  プレイヤークラス
class Player :public CollisionObject, public IPlayerParameterGetter
{
public:
    Player();
    ~Player();

    //  初期化
    void Init(const std::function <void()> _gameOver, const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass);
    //  リセット
    void Reset();
    //  外部ファイルから読み取ったデータをロード
    void LoadFileData();

    //  更新
    void Update(const XMFLOAT3& _mapSize, const float _deltaTime);

    //  描画
    void Draw();

    //  移動
    void Move();

    //  回避時の処理
    void Dodge();

    //  プレイヤーを出現させる
    void Appear();
    //  プレイヤーを退場させる
    void Exit(const float _deltaTime);

    //  プレイヤーの残機のゲッター
    int GetLife()const override { return m_life; }
    //  エネルギー量のゲッター
    int GetEnergy()const override;

    //  回避回数のゲッター
    int GetDodgeNum()const override { return m_dodgeNum; }

    //  ゲームを再開するための処理
    //  NOTE: デバッグで特定の時間からゲームを再開する際に、
    //        プレイヤーの登場演出などは省いて、
    //        座標や残機などのステータスのみを初期化する処理
#if _DEBUG
    void Restart();
#endif
private:
    //  衝突した際の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  無敵時間の更新
    void UpdateInvincibleTime();

    //  残機を減らす処理
    void ReduceLife();

    //  プレイヤーが場外へ出ないようにする
    void StayInsideMap(const XMFLOAT3& _mapSize);

    //  プレイヤーの残機
    int m_life;

    //  プレイヤーのベクトル
    XMFLOAT3 m_vec;

    //  プレイヤーのモデルを表示するか
    bool m_isDisplayModel;

    //  プレイヤーの角度
    float m_angle;

    //  デルタタイム
    float m_deltaTime;

    //  回避回数
    int m_dodgeNum;

    //  被弾エフェクトのハンドル
    int m_damageEffectHandle;
    //  回避エフェクトのハンドル
    int m_dodgeEffectHandle;
    //  退場エフェクトのハンドル
    int m_exitEffectHandle;

    //  退場開始したか
    bool m_isStartExit;

    //  ゲームオーバー関数のポインタ
    std::function <void()> m_gameOver;

    //  構造体へのポインタ
    struct MapData* m_mapData;  //  マップのデータ

    //  クラスへのポインタメンバ変数
    class Mouse* m_mouse;   //  マウスの座標
    class Timer* m_appearTimer; //  出現用時間計測
    Timer* m_invincibleTimer;   //  無敵時間計測
    Timer* m_invincibleFlashingTimer;   //  無敵時間の点滅間隔計測
    Timer* m_restartTimer;  //  リスタートの時間計測
    Timer* m_exitTimer;     //  退場時の時間計測
    class DodgeRange* m_dodgeRange;    //  回避範囲
    class Energy* m_energy; //  エネルギーの管理
    std::vector<class EnergyEffect*> m_energyEffect;        //  エネルギーのエフェクト
    std::shared_ptr<class EventNotificator> m_eventClass;   //  イベント通知

    //  シングルトンクラスへの参照
    class PMDModel& m_model;            //  PMDモデルの管理
    class EffekseerManager& m_effect;   //  Effekseerで読み込んだエフェクトの管理
    class Sound& m_sound;               //  音の管理
    class UserInputHandler& m_userInputHandler; //  入力検知

    //  プレイヤーのパラメータ
    struct Param
    {
        XMFLOAT3 pos;   //  座標
        float maxSpeed;    //  最大移動速度
        float radius;   //  衝突判定の半径
        float actualRadius; //  実際の半径
        int maxLife;    //  残機の最大値
        float invincibleTime;   //  無敵時間
    };
    Param m_param;
    
    //  プレイヤーの状態
    enum class State
    {
        NoDisplay,  //  出現していない
        Appear,         //  出現中
        Play,           //  行動中
        Damage,         //  被弾
        Invincible,     //  無敵
        Defeat          //  倒された

        , None
    };
    State m_state;
};