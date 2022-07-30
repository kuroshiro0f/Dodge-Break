#pragma once
#include <functional>
#include <list>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>

using namespace DirectX;

enum class EnemyAttackType;
//  エネミーの攻撃の生成や更新を行うクラス
class EnemyAttackDirector
{
public:
    EnemyAttackDirector();
    ~EnemyAttackDirector();

    //  初期化
    void Init();

    //  外部ファイルから読み取ったデータをロード
    void LoadFileData();

    //  更新
    void Update(const XMFLOAT3& _targetPos, const float _deltaTime);

    //  発射
    void Shoot(const EnemyAttackType _type, const int _attackPetternID, const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, float _deltaTime);

    //  攻撃の削除
    void Delete();

    //  画面上に攻撃があるか
    bool IsDisplay() { return !m_enemyAttack.empty(); }

    //  使用中の攻撃を、プール内に戻す
    void ReturnPool(class EnemyAttackBase* _class);
private:
    //  直線攻撃の発射
    void ShootStraightAttack(const int _attackPetternID, const XMFLOAT3& _pos,const XMFLOAT3& _targetPos, float _deltaTime);
    //  ファンネル攻撃の発射
    void ShootFannelAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime);
    //  ジグザグ攻撃の発射
    void ShootZigzagAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime);
    //  拡散攻撃の発射
    void ShootSpreadAttack(const int _attackPetternID, const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, float _deltaTime);
    //  弾を並べて転がす攻撃の発射
    void ShootLineAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime);
    
#if _DEBUG
    //  攻撃の実験用関数
    void ShootDebugAttack(const XMFLOAT3& _pos, float _deltaTime);
#endif

    //  RegisterPool関数へのポインタ
    std::function<void(EnemyAttackBase*)> m_returnPoolFunc;

    //  エネミー攻撃クラスを格納する変数
    std::list<EnemyAttackBase*> m_enemyAttack;
    //  m_enemyAttackのlistから削除されるエネミー攻撃クラスを保持する変数
    std::list<EnemyAttackBase*> m_deleteAttack;

    //  Poolを管理するクラスをシングルトンとしてインスタンスを保持する変数
    class EnemyAttackPool& m_pool;

    //  攻撃の発射時に必要なパラメータ
    //  直線攻撃
    struct StraightShootParam
    {
        int num;        //  一度の攻撃に使う弾数
        float angle;    //  発射時の隣の弾との間にある角度
        float firstBulletAngle; //  最初に撃つ弾の角度
    };
    std::vector<StraightShootParam> m_straightData;  //  直線攻撃
    //  ファンネル攻撃
    struct FannelShootParam
    {
        int num;        //  一度の攻撃に使う弾数
        std::vector<XMFLOAT3> shootVec; //  発射方向を決める単位ベクトル
    };
    std::vector<FannelShootParam> m_fannelData;
    //  ジグザグ攻撃
    struct ZigzagShootParam
    {
        int num;        //  一度の攻撃に使う弾数
        XMFLOAT3 shootVec; //  発射方向を決める単位ベクトル
    };
    std::vector<ZigzagShootParam> m_zigzagData;
    //  拡散攻撃
    struct SpreadShootParam
    {
        int num;        //  一度の攻撃に使う弾数
        XMFLOAT3 shootVec;  //  発射方向を決める単位ベクトル
    };
    std::vector<SpreadShootParam> m_spreadData;
    //  弾を並べて転がす攻撃
    struct LineShootParam
    {
        int num;        //  一度の攻撃に使う弾数
        XMFLOAT3 shootVec;  //  発射方向を決める単位ベクトル
    };
    std::vector<LineShootParam> m_lineData;
};