#if _DEBUG
#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  攻撃の実験用クラス
//  NOTE: 攻撃の動作などを実験するためのクラスなので、
//        実際のゲームでは動作させません
class DebugAttack :public EnemyAttackBase
{
public:
    DebugAttack();
    ~DebugAttack();

    //  更新
    void Update(const XMFLOAT3& _targetPos, const float _deltaTime)override;
    //  発射
    void Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
        , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)override;
    //  消去
    void Delete()override;
    //  外部ファイルから読み取ったデータをロード
    void LoadFileData()override {};
private:
    //  衝突時の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  移動
    void Move();

    //  移動方向への単位ベクトル
    XMFLOAT3 m_unitVec;

    //  デルタタイム
    float m_deltaTime;

    //  オブジェクトプールに戻す関数のポインタ
    std::function<void(class EnemyAttackBase*)> m_returnPoolFunc;

    //  生存時間計測クラスへのポインタ
    class Timer* m_aliveTimer;
    //  変化時間計測のポインタ
    Timer* m_changeTimer;

    float m_count;

    XMFLOAT3 m_effectPos[10];
};
#endif