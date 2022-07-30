#include "EnemyAttackBase.h"

#include <functional>

#include "CollisionType.h"
#include "EnemyAttackType.h"
#include "PMDModelType.h"

#include "Singleton.h"
#include "PMDModel.h"
#include "LoadJson.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

//  影のY座標
static const float SHADOW_POS_Y = 0.1f;

//  影の拡大率
static const float SHADOW_SCALE_MAX = 0.5f; //  最大
static const float SHADOW_SCALE_MIN = 0.2f; //  最小

//  影の拡大率の変化量
static const float SHDOW_SCALE_CHANGE_AMOUNT = 0.03f;

EnemyAttackBase::EnemyAttackBase(const EnemyAttackType _type)
    :m_shadow(Singleton<PMDModel>::GetInstance())
    , m_scaleMagnification(0)
    , m_bulletModel(Singleton<PMDModel>::GetInstance())
    , m_attackEndPosData({})
    , m_attackType(_type)
    , m_num(0)
{
}

//  初期化
void EnemyAttackBase::Init()
{
    //  衝突判定に登録
    CollisionObject::Init(CollisionType::EnemyAttack);

    //  外部ファイルから読み取ったデータをロード
    LoadFileData();

    //  外部ファイルのデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  攻撃の消える座標を外部ファイルから読み取って保存
    m_attackEndPosData.upEndPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "AttackSurvivableRangeStartPos").z;
    m_attackEndPosData.downEndPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "AttackSurvivableRangeEndPos").z;
    m_attackEndPosData.leftEndPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "AttackSurvivableRangeStartPos").x;
    m_attackEndPosData.rightEndPos = fileData.GetXMFLOAT3Data(JsonDataType::Stage, "AttackSurvivableRangeEndPos").x;

    //  モデルを使用するための設定
    //  NOTE: ジグザグ攻撃の影のモデルは攻撃自体のモデルと一緒のデータになっているため、
    //        影のモデルの設定は行わない
    if (m_attackType != EnemyAttackType::Zigzag)
    {
        m_shadow.SetUp();
    }
    m_bulletModel.SetUp();

#if _DEBUG
    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&EnemyAttackBase::LoadFileData, this));
#endif
}

//  描画
void EnemyAttackBase::Draw()
{
    //  NOTE: ジグザグ攻撃の影は攻撃自体のモデルデータと一緒になっているため、
    //        影の描画における共通処理は行わない
    if (m_isDisplay && m_attackType != EnemyAttackType::Zigzag)
    {
        //  影の拡大率を変更
        ChangeShadowScale();
        //  影の描画
        m_shadow.DrawChangePosAndScale({ m_pos.x,SHADOW_POS_Y,m_pos.z },m_scaleMagnification, PMDModelType::BulletShadow);
    }
}

//  弾が指定範囲を出たら消去する処理
void EnemyAttackBase::DeleteOutsideBullet()
{
    if (m_pos.z >= m_attackEndPosData.upEndPos || m_pos.z <= m_attackEndPosData.downEndPos
        || m_pos.x <= m_attackEndPosData.leftEndPos || m_pos.x >= m_attackEndPosData.rightEndPos)
    {
        Delete();
    }
}

//  影の拡大率をエフェクト表示位置の高さによって変える
void EnemyAttackBase::ChangeShadowScale()
{
    //  エフェクトのY座標を元に拡大率を計算
    m_scaleMagnification = SHADOW_SCALE_MAX - m_pos.y * SHDOW_SCALE_CHANGE_AMOUNT;

    //  最小値を下回らないようにする
    if (m_scaleMagnification <= SHADOW_SCALE_MIN)
    {
        m_scaleMagnification = SHADOW_SCALE_MIN;
    }
}
