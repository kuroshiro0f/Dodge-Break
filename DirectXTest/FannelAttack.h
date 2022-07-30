#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  ファンネル攻撃の挙動を行うクラス
//  NOTE: 発射後に直線に動いた後、一度停止し、その後追尾移動する攻撃を、
//        今回はとある類似する攻撃から取って、ファンネルと呼ぶ
class FannelAttack :public EnemyAttackBase
{
public:
    FannelAttack();
    ~FannelAttack();

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

    //  移動方法をStateで管理
    enum class MoveState
    {
        Straight,   //  直線軌道
        Stop,           //  停止
        Track           //  追尾軌道

        , None
    };
    MoveState m_moveState;

    //  移動方向への単位ベクトル
    XMFLOAT3 m_unitVec;

    //  パターンID
    int m_petternID;

    //  デルタタイム
    float m_deltaTime;

    //  オブジェクトプールに戻す関数のポインタ
    std::function<void(class EnemyAttackBase*)> m_returnPoolFunc;

    //  生存時間計測クラスへのポインタ
    class Timer* m_aliveTimer;
    //  変化時間計測のポインタ
    Timer* m_changeTimer;

    //  ファンネル攻撃用パラメータ
    struct Param
    {
        float aliveTime;        //  生存時間
        float shootIntervalForNumber;   //  弾の番号毎の発射間隔
        float trackinigRate;    //  追尾率

        //  発射してから止まるまで
        float timeToStop;       //  止まるまでの時間
        float speedToStop;      //  移動速度

        //  止まってから
        float timeAfterStop;    //  動き出すまでの時間
        float speedAfterStop;   //  動き出した後の移動速度
    };
    //  NOTE: パターンIDを添え字に指定する配列としてパラメータを保存
    std::vector<Param> m_param;
};