#pragma once
#include "CollisionObject.h"

#include <DirectXMath.h>
#include <functional>
#include <iostream>
#include <memory>

enum class EnemyType;
//  塔クラス
class EnemyDefeater : public CollisionObject
{
public:
    EnemyDefeater();
    ~EnemyDefeater();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass
        , const std::function <void(const EnemyType&)> _defeatFunc, int _num);
    //  リセット
    void Reset();
    //  データのロード
    void Load();

    //  更新
    void Update(const float _deltaTime);

    //  描画
    void Draw();

    //  起動
    void Activate(bool _isDisplayEnemy);
    //  停止
    void Stop();
    //  準備中に変更
    void ChangeStandBy();

    //  エネルギーを溜めているエフェクトの再生
    void PlayChargeEnergyEffect();

    //  塔が対応しているエネミーの種類を得る
    EnemyType GetEnemyDefeaterType()const { return m_type; }
    //  起動しているか
    bool IsActivate() { return m_state == State::Active; }
    //  待機中か
    bool IsStandBy()const { return m_state == State::StandBy; }
private:
    //  衝突時の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  プレイヤーとの衝突時の処理
    void OnCollisionEnterForPlayer();

    //  塔の番号
    int m_num;

    //  エフェクトハンドル
    int m_cubeEffectHandle;         //  立方体

    //  角度
    float m_angle;

    //  塔の状態
    enum class State
    {
        NoActive,   //  起動していない
        StandBy,        //  待機中（エネルギーは溜まっているが、対応した種類のエネミーがいないため起動していない状態）
        Active,         //  起動している
        Hit             //  衝突した

        , None
    };
    State m_state;

    //  塔の種類
    //  NOTE: エネミーの種類と塔の種類は関連づいているので、
    //        同じenumを使用
    EnemyType m_type;

    //  塔のパラメータ
    struct Param
    {
        float RotationSpeed;    //  立方体部分の回転速度
        float MoveSpeed;    //  立方体部分の移動速度
        float MoveDistance; //  立方体部分の移動距離
        float EffectTime;   //  エフェクトの再生間隔
        float EffectPosY;   //  エフェクトの表示座標(Y座標)
        float NecessaryEnergyAmount;    //  塔の起動に必要なエネルギー量
    };
    Param m_param;

    //  クラスへのポインタ
    class Timer* m_effectTimer;   //  エフェクトの再生間隔のタイマー
    Timer* m_defeatTimer;       //  エネミーの撃破までの時間を測るタイマー
    std::shared_ptr<EventNotificator> m_eventNotificator;

    //  エネミーを倒す関数のポインタ
    std::function <void(const EnemyType)> m_defeatEnemyFunc;

    //  シングルトンクラスへの参照
    class PMDModel& m_model;            //  PMDモデルの管理
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
    class Sound& m_sound;               //  音の管理
};