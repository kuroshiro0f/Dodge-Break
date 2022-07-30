#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  直線軌道の攻撃の挙動を行うクラス
class StraightAttack :public EnemyAttackBase
{
public:
    StraightAttack();
    ~StraightAttack();

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

    //  攻撃用パラメータ
    struct Param
    {
        float aliveTime;    //  生存時間
        float shootAngle;   //  発射方向
        float speed;        //  速度
    };
    //  NOTE: パターンIDを添え字に指定する配列としてパラメータを保存
    std::vector<Param> m_param;
};