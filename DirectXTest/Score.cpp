#include "Score.h"

#include "EnumIterator.h"
#include "ScoreType.h"
#include "EventNotificator.h"
#include "EventType.h"

#include "IPlayerParameterGetter.h"
#include "IDefeatEnemyNumGetter.h"

//  各スコアの計算時の倍率
static const int ALIVE_TIME_SCORE_MAGNIFICATION = 20;
static const int DODGE_NUM_SCORE_MAGNIFICATION = 1500;
static const int LIFE_NM_SCORE_MAGNIFICATION = 50000;
static const int DEFEAT_ENEMY_SCORE_MAGNIFICATION = 8000;

Score::Score()
    :m_playerParameterGetter(nullptr)
    , m_defeatEnemyNumGetter(nullptr)
{
    //  処理なし
}

Score::~Score()
{
    m_scoreData.clear();
    m_scoreMagnification.clear();
}

//  初期化
void Score::Init(class IPlayerParameterGetter* _playerParameterGetter, class IDefeatEnemyNumGetter* _defeatEnemyNumGetter, const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  イベント通知クラスに関数を登録
    //  回避時に回避スコアの更新を行う
    _eventClass->Register(EventType::Dodge, std::bind(&Score::UpdateDodgeScore, this));
    //  エネミーの撃破時にエネミーの撃破数スコアの更新を行う
    _eventClass->Register(EventType::DeleteEnemy, std::bind(&Score::UpdateDefeatEnemyScore, this));

    //  プレイヤーのパラメータを取得する関数のポインタを取得
    m_playerParameterGetter = _playerParameterGetter;
    //  エネミーの撃破数を取得する関数のポインタを取得
    m_defeatEnemyNumGetter = _defeatEnemyNumGetter;

    //  各スコアの計算時の倍率を設定
    m_scoreMagnification[ScoreType::AliveTime] = ALIVE_TIME_SCORE_MAGNIFICATION;
    m_scoreMagnification[ScoreType::DodgeNum] = DODGE_NUM_SCORE_MAGNIFICATION;
    m_scoreMagnification[ScoreType::LifeNum] = LIFE_NM_SCORE_MAGNIFICATION;
    m_scoreMagnification[ScoreType::DefeatEnemyNum] = DEFEAT_ENEMY_SCORE_MAGNIFICATION;
}

//  リセット
void Score::Reset()
{
    //  各スコアのリセット
    //  列挙型ScoreTypeのイテレータを作成
    typedef EnumIterator<ScoreType, ScoreType::AliveTime, ScoreType::Total> typeItr;
    for (auto itr : typeItr())
    {
        CalculateScore(itr, 0);
    }
}

//  回避スコアの更新
void Score::UpdateDodgeScore()
{
    CalculateScore(ScoreType::DodgeNum, m_playerParameterGetter->GetDodgeNum());
    m_scoreData[ScoreType::Total] = m_scoreData[ScoreType::DodgeNum] + m_scoreData[ScoreType::DefeatEnemyNum];
}
//  エネミーの撃破数スコアの更新
void Score::UpdateDefeatEnemyScore()
{
    CalculateScore(ScoreType::DefeatEnemyNum, m_defeatEnemyNumGetter->GetDefeatEnemyNum());
    m_scoreData[ScoreType::Total] = m_scoreData[ScoreType::DodgeNum] + m_scoreData[ScoreType::DefeatEnemyNum];
}

//  リザルトでの最終スコア計算
void Score::CalculateResultScore(const int _aliveTime, const int _lifeNum)
{
    //  各スコアを計算
    CalculateScore(ScoreType::AliveTime, _aliveTime);
    CalculateScore(ScoreType::DodgeNum, m_playerParameterGetter->GetDodgeNum());
    CalculateScore(ScoreType::LifeNum, _lifeNum);
    CalculateScore(ScoreType::DefeatEnemyNum, m_defeatEnemyNumGetter->GetDefeatEnemyNum());

    //  合計スコアを計算
    m_scoreData[ScoreType::Total] = m_scoreData[ScoreType::AliveTime] + m_scoreData[ScoreType::DodgeNum] + m_scoreData[ScoreType::LifeNum] + m_scoreData[ScoreType::DefeatEnemyNum];
}

//  スコア量を返す
int Score::GetScore(const ScoreType _type)
{
    return m_scoreData[_type];
}

//  スコアの配列を返す
std::unordered_map<ScoreType, int> Score::GetScoreList()
{
    return m_scoreData;
}

//  スコアの計算
void Score::CalculateScore(const ScoreType _type, const int _num)
{
    m_scoreData[_type] = _num * m_scoreMagnification[_type];
    if (m_scoreData[_type] <= 0)
    {
        m_scoreData[_type] = 0;
    }
}