#pragma once
#include "CollisionObject.h"
#include "EnemyAttackBase.h"

#include <functional>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  拡散攻撃のクラス
//  NOTE: 拡散攻撃は中心の大きな弾とその周りにくっついている弾で形成されており、
//        拡散時に中心の弾が消えて、周りの弾がくっついていた方向に飛びます。
//  NOTE: 中心の弾を0番目の弾としています
class SpreadAttack :public EnemyAttackBase
{
public:
    SpreadAttack();
    ~SpreadAttack();

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

    //  拡散したか
    bool m_isSpread;

    //  発射時の座標
    XMFLOAT3 m_firstPos;

    //  移動方向への単位ベクトル
    XMFLOAT3 m_unitVec;
    //  拡散方向への単位ベクトル
    XMFLOAT3 m_spreadUnitVec;

    //  パターンID
    int m_petternID;

    //  デルタタイム
    float m_deltaTime;

    //  オブジェクトプールに戻す関数のポインタ
    std::function<void(class EnemyAttackBase*)> m_returnPoolFunc;

    //  生存時間計測クラスへのポインタ
    class Timer* m_aliveTimer;

    //  拡散攻撃用パラメータ
    struct Param
    {
        float aliveTime;    //  生存時間
        float radius;       //  衝突判定の半径
        float centerBulletSizeMagnification;    //  中心の弾の大きさ(普通の弾に対する倍率)
        float firstMoveDistace;    //  発射時に移動しておく距離
        float speed;        //  速度
        float startSpreadDistance;  //  拡散が開始する距離
        float spreadAngle;  //  拡散するときの角度
        //  拡散角度の調整用
        //  NOTE: 拡散角度を計算する際に、この値分角度を加算した場所から角度計算をする
        float spreadStartAngle;
    };
    //  NOTE: パターンIDを添え字に指定する配列としてパラメータを保存
    std::vector<Param> m_param;
};