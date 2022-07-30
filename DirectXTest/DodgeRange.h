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