#include "DodgeRange.h"

#include "CollisionType.h"
#include "ObjectTagName.h"
#include "EventNotificator.h"
#include "EventType.h"
#include "Timer.h"

#include "Singleton.h"
#include "LoadJson.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

DodgeRange::DodgeRange()
    :m_eventNotificator(nullptr)
    , m_dodgeIntervalTimer(nullptr)
    , m_dodgeInterval(0)
    , m_collideList({})
{
    //  処理なし
}

DodgeRange::~DodgeRange()
{
    //  メモリの解放
    m_collideList.clear();
    delete m_dodgeIntervalTimer;
}

//  初期化
void DodgeRange::Init(const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  衝突判定に登録
    CollisionObject::Init(CollisionType::Sphere, ObjectTagName::DodgeRange);

    //  球状オブジェクトであるため、球状オブジェクトのデータを使用
    m_sphereData = new SphereData();

    //  回避間隔計測タイマーのインスタンスを生成
    m_dodgeIntervalTimer = new Timer();

    //  イベント通知クラスのインスタンスを取得
    m_eventNotificator = _eventClass;

    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
#if _DEBUG
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&DodgeRange::LoadFileData, this));
#endif
    //  外部ファイルからデータをロード
    LoadFileData();
}

//  リセット
void DodgeRange::Reset()
{
    //  衝突中リストのリセット
    m_collideList.clear();
    //  出現している状態から
    m_isDisplay = true;
}

//  外部ファイルから読み取ったデータをロード
void DodgeRange::LoadFileData()
{
    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  回避間隔の秒数
    //  NOTE: 連続回避ができる攻撃を回避した場合のみ使用
    m_dodgeInterval = fileData.GetFloatData(JsonDataType::Player, "DodgeInterval");

    //  衝突判定の広さ（半径）を設定
    //  NOTE: プレイヤーの衝突範囲の半径に加算
    m_sphereData->radius = fileData.GetFloatData(JsonDataType::Player, "Radius") + fileData.GetFloatData(JsonDataType::Player, "DodgeRadius");
}

//  更新
void DodgeRange::Update(const XMFLOAT3& _playerPos)
{
    //  プレイヤーを追従
    m_pos = _playerPos;

    for (auto itr = m_collideList.begin();itr != m_collideList.end();)
    {
        //  表示されなくなったオブジェクトをリスト内から検索して、
        //  見つかればリストから削除
        if (!itr->first->IsDisplay())
        {
            //  削除後に次の要素を指定して次のループへ
            itr = m_collideList.erase(itr);
            continue;
        }

        auto temp = itr->second;

        //  回避に成功時の処理
        if (temp == DodgeState::Dodge)
        {
            //  回避したことを通知
            m_eventNotificator->Notificate(EventType::Dodge);
            //  ステートを判定無視に変更
            m_collideList[itr->first] = DodgeState::NoJudge;
        }

        //  ステートを回避中に
        //  NOTE: 衝突中のオブジェクトは毎ループ衝突フラグをtrueに変えており、
        //        ループ内でtrueに変わっていないオブジェクトがあれば
        //        そのオブジェクトは範囲から出たと判断しています
        if (temp == DodgeState::Collide)
        {
            m_collideList[itr->first] = DodgeState::Dodge;
        }

        //  次の要素を指定
        itr++;
    }
}

//  回避失敗時の処理
void DodgeRange::DodgeFailed(const CollisionObject& _class)
{
    //  プレイヤーに衝突したオブジェクトを検索して、
    //  見つかればステートを判定無視に変更
    auto itr = m_collideList.find(&_class);
    if (itr != m_collideList.end())
    {
        m_collideList[itr->first] = DodgeState::NoJudge;
    }
}
//  無敵時の更新
void DodgeRange::UpdateInvincible(const XMFLOAT3& _playerPos)
{
    //  プレイヤーを追従
    m_pos = _playerPos;
    for (auto itr = m_collideList.begin(); itr != m_collideList.end();)
    {
        //  表示されなくなったオブジェクトをリスト内から検索して、
        //  見つかればリストから削除
        if (!itr->first->IsDisplay())
        {
            //  削除後に次の要素を指定して次のループへ
            itr = m_collideList.erase(itr);
            continue;
        }

        //  無敵中に衝突したオブジェクトは衝突した判定にしない
        if (itr->second == DodgeState::Collide)
        {
            m_collideList[itr->first] = DodgeState::NoCollide;
        }

        //  次の要素を指定
        itr++;
    }
}

//  衝突時の処理
void DodgeRange::OnCollisionEnter(const CollisionObject& _class)
{
    //  ビーム攻撃は連続回避処理を行う
    if (_class.GetObjectTagName() == ObjectTagName::BeamAttack)
    {
        ConinuousDodge();
    }
    //  上記以外は回避範囲に衝突後、プレイヤーに衝突せずに離れたら回避するように
    else
    {
        RegisterOneTimeDodgeObjectCollideList(_class);
    }
}

//  連続回避処理用の関数
void DodgeRange::ConinuousDodge()
{
    //  最初の回避か、直前の回避から一定時間経過していれば
    if (m_dodgeIntervalTimer->GetElapseTime() >= m_dodgeInterval || !m_dodgeIntervalTimer->IsStart())
    {
        //  回避したことを通知
        m_eventNotificator->Notificate(EventType::Dodge);
        m_dodgeIntervalTimer->Start();
    }
}

//  一度のみ回避するオブジェクトが衝突中になった際にリストに登録する処理
void DodgeRange::RegisterOneTimeDodgeObjectCollideList(const CollisionObject& _class)
{
    //  衝突したオブジェクトを検索して、判定済でなければステートを衝突中に
    auto itr = m_collideList.find(&_class);
    if (itr != m_collideList.end())
    {
        if (itr->second != DodgeState::NoJudge)
        {
            itr->second = DodgeState::Collide;
        }
    }
    //  検索した結果、オブジェクトが存在しなかった場合は
    //  衝突したオブジェクトをリストに登録
    else
    {
        m_collideList[&_class] = DodgeState::Collide;
    }
}
