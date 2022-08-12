#pragma once
#include "CollisionObject.h"

#include <DirectXMath.h>
#include <unordered_map>
#include <functional>
#include <memory>

using namespace DirectX;

enum class ChangeEnergyType;
//  回避範囲そのものを実体化したクラス
class DodgeRange :public CollisionObject
{
public:
    DodgeRange();
    ~DodgeRange();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  更新
    void Update(const XMFLOAT3& _playerPos);
    //  無敵時の更新
    void UpdateInvincible(const XMFLOAT3& _playerPos);

    //  回避失敗時の処理
    void DodgeFailed(const CollisionObject& _class);

private:
    //  衝突時の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  外部ファイルから読み取ったデータをロード
    void LoadFileData();

    //  連続回避処理
    void ConinuousDodge();

    //  一度のみ回避するオブジェクトが衝突中になった際にリストに登録する処理
    void RegisterOneTimeDodgeObjectCollideList(const CollisionObject& _class);

    //  回避間隔を計測するためのタイマークラスへのポインタ
    //  NOTE: 連続回避ができる攻撃を回避した場合のみ使用
    class Timer* m_dodgeIntervalTimer;
    //  回避間隔
    //  NOTE: 外部ファイルで値を設定
    //  NOTE: 連続回避ができる攻撃を回避した場合のみ使用
    float m_dodgeInterval;

    //  イベント通知クラスへのポインタ
    std::shared_ptr<EventNotificator> m_eventNotificator;

    //  回避判定のステート
    enum class DodgeState
    {
        Collide,    //  衝突中
        NoCollide,  //  衝突していない
        Dodge,      //  回避中
        NoJudge     //  判定無視

        , None
    };

    //  衝突したオブジェクトのリスト
    std::unordered_map<const CollisionObject*, DodgeState> m_collideList;
};