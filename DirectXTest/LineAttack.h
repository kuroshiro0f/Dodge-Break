#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  弾を並べて転がす攻撃のクラス
//  NOTE: 攻撃の流れは、
//        → 弾を順番に発射 
//        → 一定距離移動したら等間隔で停止 
//        → 一定時間経過後に同じ方向へ一斉に移動
class LineAttack :public EnemyAttackBase
{
public:
    LineAttack();
    ~LineAttack();

    //  更新
    void Update(const XMFLOAT3& _targetPos, const float _deltaTime)override;
    //  発射
    void Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
        , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc)override;
    //  消去
    void Delete()override;
    //  外部ファイルから読み取ったデータをロード
    void LoadFileData()override;
private:
    //  衝突時の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  移動
    void Move();

    //  弾を並べて転がす攻撃の状態
    enum class LineAttackState
    {
        NoShoot,    //  未発射
        Shoot,          //  発射
        Align,          //  整列する
        Stop,           //  停止
        Move            //  移動

        , None
    };
    LineAttackState m_state;

    //  発射時の座標
    XMFLOAT3 m_firstPos;

    //  移動方向への単位ベクトル
    XMFLOAT3 m_unitVec;

    //  移動速度
    float m_speed;

    //  止まるまでに動く距離
    float m_stopDistance;

    //  パターンID
    int m_petternID;

    //  デルタタイム
    float m_deltaTime;

    //  オブジェクトプールに戻す関数のポインタ
    std::function<void(class EnemyAttackBase*)> m_returnPoolFunc;

    //  生存時間計測クラスへのポインタ
    class Timer* m_aliveTimer;
    Timer* m_changeTimer;

    //  弾を並べて転がす攻撃用パラメータ
    struct Param
    {
        float aliveTime;    //  生存時間
        float alignSpeed;   //  整列速度
        float speed;        //  速度
        float shootIntervalForNumber;       //  弾同士の発射間隔
        float stopDistance;     //  停止までに移動する距離
        float distanceIntervalForNumber;    //  弾同士の距離
        float timeForRestartMove;   //  移動再開までの時間
        XMFLOAT3 rollVec;       //  弾の転がる向き
    };
    //  NOTE: パターンIDを添え字に指定する配列としてパラメータを保存
    std::vector<Param> m_param;
};