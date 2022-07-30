#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
class ZigzagAttack :public EnemyAttackBase
{
public:
    ZigzagAttack();
    ~ZigzagAttack();

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
    //  変化時間計測のポインタ
    Timer* m_changeTimer;
    //  発射開始時間計測のポインタ
    Timer* m_shootTimer;

    //  ジグザグ攻撃の状態
    enum class ZigzagState
    {
        NoShoot,    //  撃たれていない
        Shoot,          //  撃たれた
        EndAliveTime,   //  生存時間が終了した
        Hit         //  プレイヤーと衝突した

        , None
    };
    ZigzagState m_state;

    //  ジグザグ攻撃のパラメータ
    struct Param
    {
        float aliveTime;    //  生存時間
        float shootIntervalForNumber;   //  弾の番号毎の発射間隔
        float speed;        //  速度
        float angle;        //  角度
        float addPosX;      //  x座標の加算量
        float addPosZ;      //  z座標の加算量
    };
    //  NOTE: パターンIDを添え字に指定する配列としてパラメータを保存
    std::vector<Param> m_param;
};