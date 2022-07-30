#pragma once
#include "CollisionObject.h"

#include <memory>
#include <unordered_map>

enum class EnemyType;
//  プレイヤーが回避した際にプレイヤーから塔に飛ぶエネルギーのエフェクト
class EnergyEffect
{
public:
    EnergyEffect();
    ~EnergyEffect();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass);
    //  リセット
    void Reset();

    //  更新
    void Update(const float _deltaTime);

    //  発射
    void Shoot(const XMFLOAT3 _pos,const bool _isEnergyMax);

    //  出現中か
    bool IsDisplay()const { return m_state != State::NoDispay; }
private:
    //  移動
    void Move();

    //  エフェクトの状態
    enum class State
    {
        Spread, //  拡散
        GoTower,    //  塔へ移動
        NoDispay    //  出現していない

        , None
    };
    State m_state;

    //  現在の座標
    std::unordered_map<EnemyType,XMFLOAT3> m_pos;
    //  発射時の座標
    XMFLOAT3 m_firstPos;
    //  ベジェ曲線を使った移動における制御点
    std::unordered_map<EnemyType,XMFLOAT3> m_controlPoint;

    //  移動割合
    //  NOTE:ベジェ曲線を用いて移動するため、移動量は速度ではなく割合で計算
    float m_moveRate;
    //  角度
    std::unordered_map<EnemyType,float> m_angle;

    //  エネルギーは満タンか
    bool m_isEnergyMax;

    //  デルタタイム
    float m_deltaTime;

    //  エフェクトハンドル
    std::unordered_map<EnemyType,int> m_effectHandle;

    //  拡散時間の計測
    class Timer* m_spreadTimer;
    //  塔の位置を知るためのクラス
    std::shared_ptr<class EnemyDefeaterMapper> m_enemyDefeaterMapperClass;
    //  イベント通知を行うクラス
    std::shared_ptr<class EventNotificator> m_eventClass;

    //  エフェクトを表示するためのクラスをシングルトンとしてインスタンスを保持する変数
    class EffekseerManager& m_effect;
};