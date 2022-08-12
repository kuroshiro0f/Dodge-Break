#pragma once
#include <deque>
#include <functional>

enum class EnemyAttackType;
//  エネミー攻撃クラスを格納し、状態を管理するオブジェクトプールクラス
//  シングルトンで扱う
class EnemyAttackPool
{
public:
    EnemyAttackPool();
    ~EnemyAttackPool();

    //  初期化
    void Init();

    //  待機中のエネミー攻撃クラスのインスタンスを返す
    class EnemyAttackBase* GetEnemyAttack(const EnemyAttackType _type);

    //  待機中のビーム攻撃クラスのインスタンスを返す
    class BeamAttack* GetBeamAttack();

    //  使用されなくなったインスタンスを待機中として再登録
    void RegisterStandByEnemyAttack(EnemyAttackBase* _class);
    //  使用されなくなったビーム攻撃クラスのインスタンスを待機中として再登録
    void RegisterStandByEnemyAttack(BeamAttack* _beamAttackClass);
private:
    //  Poolにエネミー攻撃クラスのインスタンスを登録
    void RegisterPool(const EnemyAttackType _type);

    //  エネミー攻撃クラスを格納するクラス
    std::deque<EnemyAttackBase*> m_straightAttack;  //  直線軌道
    std::deque<EnemyAttackBase*> m_fannelAttack;    //  ファンネル
    std::deque<EnemyAttackBase*> m_zigzagAttack;    //  ジグザグ軌道
    std::deque<EnemyAttackBase*> m_spreadAttack;    //  拡散
    std::deque<EnemyAttackBase*> m_lineAttack;      //  弾を並べて転がす

    //  ビーム攻撃クラスを格納するクラス
    //  NOTE: BeamAttackクラスはEnemyAttackBaseクラスを継承していないため、BeamAttackクラスへのポインタの配列として宣言
    std::deque<class BeamAttack*> m_beamAttack;

#if _DEBUG
    std::deque<EnemyAttackBase*> m_debugAttack;     //  攻撃の実験用クラス
#endif
};