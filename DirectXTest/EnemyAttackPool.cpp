#include "EnemyAttackPool.h"

#include <string>

#include "EnemyAttackType.h"
#include "EnemyAttackBase.h"
#include "StraightAttack.h"
#include "FannelAttack.h"
#include "ZigzagAttack.h"
#include "SpreadAttack.h"
#include "LineAttack.h"
#include "EnumIterator.h"

#include "Singleton.h"
#include "CheckError.h"
#include "LoadJson.h"

#if _DEBUG
#include "DebugAttack.h"
#endif

EnemyAttackPool::EnemyAttackPool()
    :m_straightAttack({})
    , m_fannelAttack({})
    , m_zigzagAttack({})
    , m_spreadAttack({})
    , m_lineAttack({})
#if _DEBUG
    , m_debugAttack({})
#endif
{
}

EnemyAttackPool::~EnemyAttackPool()
{
    for (auto itr : m_straightAttack)
    {
        delete itr;
    }
    m_straightAttack.clear();
    m_straightAttack.shrink_to_fit();

    for (auto itr : m_fannelAttack)
    {
        delete itr;
    }
    m_fannelAttack.clear();
    m_fannelAttack.shrink_to_fit();

    for (auto itr : m_zigzagAttack)
    {
        delete itr;
    }
    m_zigzagAttack.clear();
    m_zigzagAttack.shrink_to_fit();

    for (auto itr : m_spreadAttack)
    {
        delete itr;
    }
    m_spreadAttack.clear();
    m_spreadAttack.shrink_to_fit();

    for (auto itr : m_lineAttack)
    {
        delete itr;
    }
    m_lineAttack.clear();
    m_lineAttack.shrink_to_fit();

#if _DEBUG
    for (auto itr : m_debugAttack)
    {
        delete itr;
    }
    m_debugAttack.clear();
    m_debugAttack.shrink_to_fit();
#endif
}

//  初期化
void EnemyAttackPool::Init()
{
#if _DEBUG
    //  列挙型EnemyAttackOrbitTypeのイテレータを作成
    typedef EnumIterator<EnemyAttackType, EnemyAttackType::Straight, EnemyAttackType::Debug> typeItr;
#else
    //  列挙型EnemyAttackOrbitTypeのイテレータを作成
    typedef EnumIterator<EnemyAttackType, EnemyAttackType::Straight, EnemyAttackType::NoAttack> typeItr;
#endif
    //  タイプごとにエネミー攻撃クラスをプールに登録
    for (auto itr : typeItr())
    {
        RegisterPool(itr);
    }
}

//  Poolにエネミー攻撃クラスのインスタンスを登録
void EnemyAttackPool::RegisterPool(const EnemyAttackType _type)
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();
    //  ロードする攻撃の数
    int attackNum;
    switch (_type)
    {
    case EnemyAttackType::Straight:
        //  外部ファイルを読み取ってロード数を設定
        attackNum = fileData.GetIntData(JsonDataType::StraightAttack, "TotalNum");
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_straightAttack.emplace_back(new StraightAttack());
            m_straightAttack.back()->Init();
        }
        break;
    case EnemyAttackType::Fannel:
        //  外部ファイルを読み取ってロード数を設定
        attackNum = fileData.GetIntData(JsonDataType::FannelAttack, "TotalNum");
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_fannelAttack.emplace_back(new FannelAttack());
            m_fannelAttack.back()->Init();
        }
        break;
    case EnemyAttackType::Zigzag:
        //  外部ファイルを読み取ってロード数を設定
        attackNum = fileData.GetIntData(JsonDataType::ZigzagAttack, "TotalNum");
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_zigzagAttack.emplace_back(new ZigzagAttack());
            m_zigzagAttack.back()->Init();
        }
        break;
    case EnemyAttackType::Spread:
        //  外部ファイルを読み取ってロード数を設定
        attackNum = fileData.GetIntData(JsonDataType::SpreadAttack, "TotalNum");
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_spreadAttack.emplace_back(new SpreadAttack());
            m_spreadAttack.back()->Init();
        }
        break;
    case EnemyAttackType::Line:
        //  外部ファイルを読み取ってロード数を設定
        attackNum = fileData.GetIntData(JsonDataType::LineAttack, "TotalNum");
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_lineAttack.emplace_back(new LineAttack());
            m_lineAttack.back()->Init();
        }
        break;
#if _DEBUG
    case EnemyAttackType::Debug:
        //  デバッグ用の仮の数を代入
        attackNum = 100;
        //  指定された種類の攻撃クラスのインスタンスをコンテナに格納していく
        for (int i = 0; i < attackNum; i++)
        {
            m_debugAttack.emplace_back(new DebugAttack());
            m_debugAttack.back()->Init();
        }
        break;
#endif
    default:
        break;
    }
}

//  エネミー攻撃クラスのインスタンスを返す
EnemyAttackBase* EnemyAttackPool::GetEnemyAttack(const EnemyAttackType _type)
{
    switch (_type)
    {
    case EnemyAttackType::Straight:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_straightAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_straightAttack.front();
            m_straightAttack.pop_front();
            return retEnemyAttack;
        }
        break;
    case EnemyAttackType::Fannel:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_fannelAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_fannelAttack.front();
            m_fannelAttack.pop_front();
            return retEnemyAttack;
        }
        break;
    case EnemyAttackType::Zigzag:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_zigzagAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_zigzagAttack.front();
            m_zigzagAttack.pop_front();
            return retEnemyAttack;
        }
        break;
    case EnemyAttackType::Spread:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_spreadAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_spreadAttack.front();
            m_spreadAttack.pop_front();
            return retEnemyAttack;
        }
        break;
    case EnemyAttackType::Line:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_lineAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_lineAttack.front();
            m_lineAttack.pop_front();
            return retEnemyAttack;
        }
        break;
#if _DEBUG
    case EnemyAttackType::Debug:
        //  格納しているエネミーの攻撃が全て使用されているなら、エラーメッセージを出してゲームを停止
        if (m_debugAttack.empty())
        {
            CheckError& error = Singleton<CheckError>::GetInstance();
            std::string errorMassage = "EnemyAttackPool内に格納しているエネミーの攻撃が足りませんでした。";
            error.CreateErrorMessage(errorMassage);
        }

        {
            //  返すインスタンスを決定しつつ、次のインスタンスをセット
            auto retEnemyAttack = m_debugAttack.front();
            m_debugAttack.pop_front();
            return retEnemyAttack;
        }
        break;
#endif
    default:
        break;
    }
}

//  使用されなくなったインスタンスを待機中として再登録
void EnemyAttackPool::RegisterStandByEnemyAttack(EnemyAttackBase* _class)
{
    switch (_class->GetType())
    {
    case EnemyAttackType::Straight:
        m_straightAttack.emplace_back(_class);
        break;
    case EnemyAttackType::Fannel:
        m_fannelAttack.emplace_back(_class);
        break;
    case EnemyAttackType::Zigzag:
        m_zigzagAttack.emplace_back(_class);
        break;
    case EnemyAttackType::Spread:
        m_spreadAttack.emplace_back(_class);
        break;
    case EnemyAttackType::Line:
        m_lineAttack.emplace_back(_class);
        break;
#if _DEBUG
    case EnemyAttackType::Debug:
        m_debugAttack.emplace_back(_class);
        break;
#endif
    default:
        break;
    }
}