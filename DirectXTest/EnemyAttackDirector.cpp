#include "EnemyAttackDirector.h"

#include <random>

#include "EnemyAttackType.h"
#include "EnemyAttackBase.h"
#include "BeamAttack.h"
#include "EnumIterator.h"
#include "XMFLOATHelper.h"

#include "Singleton.h"
#include "EnemyAttackPool.h"
#include "LoadJson.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

EnemyAttackDirector::EnemyAttackDirector()
    :m_pool(Singleton<EnemyAttackPool>::GetInstance())
    , m_returnPoolFunc(nullptr)
    , m_enemyAttack({})
    , m_deleteAttack({})
    , m_beamAttackList({})
    , m_deleteBeamAttackList({})
    , m_straightData({})
    , m_fannelData({})
    , m_zigzagData({})
    , m_spreadData({})
    , m_lineData({})
    , m_isAttack(false)
{
    //  処理なし
}

EnemyAttackDirector::~EnemyAttackDirector()
{
    m_deleteAttack.clear();
    m_enemyAttack.clear();
    m_beamAttackList.clear();
    m_deleteBeamAttackList.clear();
}

//  初期化
void EnemyAttackDirector::Init()
{
    //  外部ファイルから読み取ったデータをロード
    LoadFileData();

    //  オブジェクトプールにエネミーの攻撃を戻すための関数を設定
    m_returnPoolFunc = std::bind(&EnemyAttackDirector::ReturnPool, this, std::placeholders::_1);
    m_returnPoolForBeamAttackFunc = std::bind(&EnemyAttackDirector::ReturnPoolForBeamAttack, this, std::placeholders::_1);

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&EnemyAttackDirector::LoadFileData, this));
#endif
}

//  外部ファイルから読み取ったデータをロード
void EnemyAttackDirector::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  直線攻撃用のデータを設定
    int num = fileData.GetArraySize(JsonDataType::StraightAttack, "StraightAttackData");
    for (int i = 0; i < num; i++)
    {
        StraightShootParam temp;
        //  一度の攻撃に使う弾数
        temp.num = fileData.GetIntData(JsonDataType::StraightAttack, "StraightAttackData", i, "WayNum");
        //  発射時の隣の弾との間にある角度
        temp.angle = fileData.GetFloatData(JsonDataType::StraightAttack, "StraightAttackData", i, "ShootAngle");
        //  最初に撃つ弾の角度決定
        temp.firstBulletAngle = -(temp.num - 1) * temp.angle * 0.5f;
        
        m_straightData.emplace_back(temp);
    }

    //  ファンネル攻撃用のデータを設定
    num = fileData.GetArraySize(JsonDataType::FannelAttack, "FannelAttackData");
    for (int i = 0; i < num; i++)
    {
        FannelShootParam temp;
        //  一度の攻撃に使う弾数
        temp.num = fileData.GetIntData(JsonDataType::FannelAttack, "FannelAttackData", i, "Num");
        //  発射方向を決める単位ベクトル
        int size = fileData.GetArraySize(JsonDataType::FannelAttack, "FannelAttackData", i, "ShootVec");
        for (int j = 0; j < size; j++)
        {
            temp.shootVec.emplace_back(fileData.GetXMFLOAT3Data(JsonDataType::FannelAttack, "FannelAttackData", i, "ShootVec", j));
        }

        m_fannelData.emplace_back(temp);
    }

    //  ジグザグ攻撃用のデータを設定
    num = fileData.GetArraySize(JsonDataType::ZigzagAttack, "ZigzagAttackData");
    for (int i = 0; i < num; i++)
    {
        ZigzagShootParam temp;
        //  一度の攻撃に使う弾数
        temp.num = fileData.GetIntData(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "Num");
        //  発射方向を決める単位ベクトル
        temp.shootVec = fileData.GetXMFLOAT3Data(JsonDataType::ZigzagAttack, "ZigzagAttackData", i, "ShootVec");

        m_zigzagData.emplace_back(temp);
    }

    //  拡散攻撃用のデータを設定
    num = fileData.GetArraySize(JsonDataType::SpreadAttack, "SpreadAttackData");
    for (int i = 0; i < num; i++)
    {
        SpreadShootParam temp;
        //  一度の攻撃に使う弾数
        temp.num = fileData.GetIntData(JsonDataType::SpreadAttack, "SpreadAttackData", i, "Num");
        //  発射方向を決める単位ベクトル
        //  NOTE: プレイヤーの位置を参照して発射方向を決定するため、
        //        値を使用しないので0で初期化
        XMFLOAT3 tempPos = { 0,0,0 };
        temp.shootVec = tempPos;

        m_spreadData.emplace_back(temp);
    }

    //  弾を並べて転がす攻撃のデータを設定
    num = fileData.GetArraySize(JsonDataType::LineAttack, "LineAttackData");
    for (int i = 0; i < num; i++)
    {
        LineShootParam temp;
        //  一度の攻撃に使う弾数
        temp.num = fileData.GetIntData(JsonDataType::LineAttack, "LineAttackData", i, "Num");
        //  発射方向を決める単位ベクトル
        temp.shootVec = fileData.GetXMFLOAT3Data(JsonDataType::LineAttack, "LineAttackData", i, "ShootVec");

        m_lineData.emplace_back(temp);
    }
}

//  更新
void EnemyAttackDirector::Update(const XMFLOAT3& _targetPos, const float _deltaTime)
{
    //  各攻撃の更新と描画
    for (auto itr : m_enemyAttack)
    {
        itr->Update(_targetPos, _deltaTime);
        itr->DeleteOutsideBullet();
        itr->Draw();
    }
    for (auto itr : m_beamAttackList)
    {
        itr->Update(_deltaTime);
    }

    //  Poolに再登録されたインスタンスをlistから削除
    //  NOTE: Delete関数内で行うと、m_enemyAttackやm_beamAttackListのイテレータを使ってUpdateを順番に実行している際に、
    //        listのサイズが変化してしまい、ループにエラーが発生してしまうため、
    //        list内の全Update実行後にまとめて削除
    for (auto itr : m_deleteAttack)
    {
        auto findItr = std::find(m_enemyAttack.begin(), m_enemyAttack.end(), itr);
        m_enemyAttack.erase(findItr);
    }
    m_deleteAttack.clear();
    for (auto itr : m_deleteBeamAttackList)
    {
        auto findItr = std::find(m_beamAttackList.begin(), m_beamAttackList.end(), itr);
        m_beamAttackList.erase(findItr);
    }
    m_deleteBeamAttackList.clear();

    //  処理中の攻撃がなければ攻撃中か確認するフラグをfalseに
    if (m_enemyAttack.empty() && m_beamAttackList.empty())
    {
        m_isAttack = false;
    }
}

//  生成して攻撃を開始する
void EnemyAttackDirector::Shoot(const EnemyAttackType _type, const int _attackPetternID, const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, float _deltaTime)
{
    switch (_type)
    {
    case EnemyAttackType::Straight:
        ShootStraightAttack(_attackPetternID, _pos, _targetPos, _deltaTime);
        break;
    case EnemyAttackType::Fannel:
        ShootFannelAttack(_attackPetternID, _pos, _deltaTime);
        break;
    case EnemyAttackType::Zigzag:
        ShootZigzagAttack(_attackPetternID, _pos, _deltaTime);
        break;
    case EnemyAttackType::Spread:
        ShootSpreadAttack(_attackPetternID, _pos, _targetPos, _deltaTime);
        break;
    case EnemyAttackType::Line:
        ShootLineAttack(_attackPetternID, _pos, _deltaTime);
        break;
    case EnemyAttackType::Beam:
        ShootBeamAttack(_attackPetternID, _pos);
        break;
#if _DEBUG
    case EnemyAttackType::Debug:
        ShootDebugAttack(_pos, _deltaTime);
        break;
#endif
    default:
        break;
    }

    //  攻撃中か確認するフラグをtrueに
    m_isAttack = true;
}

//  攻撃の削除
void EnemyAttackDirector::Delete()
{
    for (auto itr : m_enemyAttack)
    {
        itr->Delete();
    }
    for (auto itr : m_beamAttackList)
    {
        itr->Delete();
    }

    m_isAttack = false;
}

//  使用中の攻撃を、プール内に戻す
void EnemyAttackDirector::ReturnPool(EnemyAttackBase* _class)
{
    m_pool.RegisterStandByEnemyAttack(_class);
    m_deleteAttack.emplace_back(_class);
}
//  使用中のビーム攻撃を、プール内に戻す
void EnemyAttackDirector::ReturnPoolForBeamAttack(BeamAttack* _beamAttackClass)
{
    m_pool.RegisterStandByEnemyAttack(_beamAttackClass);
    m_deleteBeamAttackList.emplace_back(_beamAttackClass);
}

//  直線攻撃の発射
void EnemyAttackDirector::ShootStraightAttack(const int _attackPetternID, const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, float _deltaTime)
{
    //  規定数の直線軌道の攻撃をEnemyAttackPool内から取り出して、発射する
    int num = m_straightData[_attackPetternID].num;
    for (int i = 0; i < num; i++)
    {
        //  攻撃のベクトルの決定
        XMFLOAT3 unitVec = XMFLOATHelper::XMFLOAT3TiltXZ(XMFLOATHelper::XMFLOAT3Normalize(_pos, _targetPos), m_straightData[_attackPetternID].firstBulletAngle + m_straightData[_attackPetternID].angle * i);

        //  直線軌道の攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Straight));
        //  発射
        m_enemyAttack.back()->Shoot(_attackPetternID, _pos, unitVec, i, _deltaTime, m_returnPoolFunc);
    }
}

//  ファンネル攻撃の発射
void EnemyAttackDirector::ShootFannelAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime)
{
    int num = m_fannelData[_attackPetternID].num;
    for (int i = 0; i < num; i++)
    {
        //  ファンネル攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Fannel));
        //  発射
        m_enemyAttack.back()->Shoot(_attackPetternID, _pos, m_fannelData[_attackPetternID].shootVec[i], i, _deltaTime, m_returnPoolFunc);
    }
}

//  ジグザグ攻撃の発射
void EnemyAttackDirector::ShootZigzagAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime)
{
    int num = m_zigzagData[_attackPetternID].num;
    for (int i = 0; i < num; i++)
    {
        //  攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Zigzag));
        //  発射
        m_enemyAttack.back()->Shoot(_attackPetternID, _pos, m_zigzagData[_attackPetternID].shootVec, i, _deltaTime, m_returnPoolFunc);
    }
}

//  拡散攻撃の発射
void EnemyAttackDirector::ShootSpreadAttack(const int _attackPetternID, const XMFLOAT3& _pos, const XMFLOAT3& _targetPos, float _deltaTime)
{
    int num = m_spreadData[_attackPetternID].num;
    //  攻撃のベクトルの決定
    m_spreadData[_attackPetternID].shootVec = XMFLOATHelper::XMFLOAT3Normalize(_pos, _targetPos);
    for (int i = 0; i < num; i++)
    {
        //  攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Spread));
        //  発射
        m_enemyAttack.back()->Shoot(_attackPetternID, _pos, m_spreadData[_attackPetternID].shootVec, i, _deltaTime, m_returnPoolFunc);
    }
}

//  弾を並べて転がす攻撃の発射
void EnemyAttackDirector::ShootLineAttack(const int _attackPetternID, const XMFLOAT3& _pos, float _deltaTime)
{
    int num = m_lineData[_attackPetternID].num;
    for (int i = 0; i < num; i++)
    {
        //  攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Line));
        //  発射
        m_enemyAttack.back()->Shoot(_attackPetternID, _pos, m_lineData[_attackPetternID].shootVec, i, _deltaTime, m_returnPoolFunc);
    }
}

//  ビーム攻撃の発射
void EnemyAttackDirector::ShootBeamAttack(const int _attackPetternID, const XMFLOAT3& _pos)
{
    //  ビーム攻撃をEnemyAttackPool内から取り出す
    m_beamAttackList.emplace_back(m_pool.GetBeamAttack());
    //  発射
    m_beamAttackList.back()->Shoot(_attackPetternID, _pos, m_returnPoolForBeamAttackFunc);
}

#if _DEBUG
//  攻撃の実験用関数
void EnemyAttackDirector::ShootDebugAttack(const XMFLOAT3& _pos, float _deltaTime)
{
    //  攻撃のベクトルの決定
    XMFLOAT3 unitVec = { 0,0,-1 };
    //  実験用の仮の個数分弾を生成
    for (int i = 0; i < 4; i++)
    {
        //  攻撃をEnemyAttackPool内から取り出す
        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackType::Debug));
        //  発射
        m_enemyAttack.back()->Shoot(0, _pos, unitVec, i, _deltaTime, m_returnPoolFunc);
    }
}
////  攻撃の実験用関数
//void EnemyAttackDirector::ShootDebugAttack(const XMFLOAT3& _pos, float _deltaTime)
//{
//    //  攻撃のベクトルの決定
//    XMFLOAT3 unitVec = m_zigzagData.shootVec[0];
//    //  実験用の仮の個数分弾を生成
//    for (int i = 0; i < 3; i++)
//    {
//        //  攻撃をEnemyAttackPool内から取り出す
//        m_enemyAttack.emplace_back(m_pool.GetEnemyAttack(EnemyAttackOrbitType::Debug));
//        //  発射
//        m_enemyAttack.back()->Shoot(EnemyAttackType::Debug, _pos, unitVec, i, _deltaTime, m_returnPoolFunc);
//    }
//}
#endif