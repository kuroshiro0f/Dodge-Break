#pragma once
#include "CollisionObject.h"

#include <functional>
#include <DirectXMath.h>

#include "EffekseerManager.h"

using namespace DirectX;

enum class EnemyAttackType;
enum class EffectType;
//  エネミーの攻撃のベースクラス
//  EnemyAttackDirectorクラスがこのクラスの関数を呼ぶことで、継承先のクラスの関数が呼び出されて、
//  各攻撃が発生する
class EnemyAttackBase :public CollisionObject
{
public:
    EnemyAttackBase(const EnemyAttackType _type);
    virtual ~EnemyAttackBase() {};

    //  初期化
    void Init();

    //  更新
    virtual void Update(const XMFLOAT3& _targetPos, const float _deltaTime) = 0;
    //  発射
    //  NOTE: EnemyAttackBaseはオブジェクトプールとしてシングルトンに管理されており、
    //        コンストラクタで値を渡すことができないため、
    //        発射の際にEnemyAttackDirectorクラスのDelete関数へのポインタを渡す
    virtual void Shoot(const int _petternID, const XMFLOAT3& _pos, const XMFLOAT3& _unitVec, int _num
        , float _deltaTime, std::function<void(class EnemyAttackBase*)> _returnPoolFunc) = 0;
    //  消去
    virtual void Delete() = 0;

    //  外部ファイルから読み取ったデータをロード
    virtual void LoadFileData() = 0;

    //  描画
    //  NOTE: 共通の処理なので、Baseクラスで定義
    void Draw();

    //  弾が指定範囲を出たら消去する処理
    void DeleteOutsideBullet();

    //  攻撃の種類のゲッター
    EnemyAttackType GetType()const { return m_attackType; }
protected:
    //  その攻撃内での自分の番号
    //  NOTE: 基本的に複数の弾で攻撃が形成されているため、
    //        それぞれの弾の軌道をこの番号ごとに管理する
    int m_num;

    //  影のモデル
    class PMDModel& m_shadow;
    //  弾のモデル
    PMDModel& m_bulletModel;
private:
    //  影の拡大率をエフェクト表示位置の高さによって変える
    void ChangeShadowScale();
    //  影の拡大率
    float m_scaleMagnification;

    //  攻撃の種類
    EnemyAttackType m_attackType;

    //  攻撃が消える座標のデータ
    struct AttackEndPosData
    {
        float upEndPos;     //  上端の座標
        float downEndPos;   //  下端の座標
        float leftEndPos;   //  左端の座標
        float rightEndPos;  //  右端の座標
    };
    AttackEndPosData m_attackEndPosData;
};