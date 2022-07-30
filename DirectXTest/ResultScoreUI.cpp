#include "ResultScoreUI.h"

#include <DirectXMath.h>

#include "ScoreType.h"
#include "EnumIterator.h"
#include "StringDrawer.h"
#include "SoundType.h"
#include "ResultScoreUIData.h"

#include "IScoreGetter.h"

#include "Singleton.h"
#include "Sound.h"

using namespace DirectX;

ResultScoreUI::ResultScoreUI()
    :m_string(new StringDrawer())
    , m_isFinishDisplayScore(false)
    , m_sound(Singleton<Sound>::GetInstance())
{
}

ResultScoreUI::~ResultScoreUI()
{
    delete m_string;
}

//  リセット
void ResultScoreUI::Reset()
{
    //  列挙型ScoreTypeのイテレータを作成
    typedef EnumIterator<ScoreType, ScoreType::AliveTime, ScoreType::Total> typeItr;
    //  表示上の各スコアのリセット
    for (auto itr : typeItr())
    {
        m_displayScore[itr] = 0;
    }
    //  スコア表示はまだ終わっていない状態から
    m_isFinishDisplayScore = false;
}

//  スコアの設定
void ResultScoreUI::SetUp(class IScoreGetter* _score)
{
    m_actualScore = _score->GetScoreList();
    m_displayScoreType = ScoreType::AliveTime;
}

//  更新
void ResultScoreUI::Update()
{
    //  アニメーション
    PlayAnimationScore(m_displayScoreType);
    //  表示されているスコアが、実際のスコアと同値になった場合、次のスコアの表示に切り替える
    if (m_displayScore[m_displayScoreType] == m_actualScore[m_displayScoreType])
    {
        if (m_displayScoreType != ScoreType::Total)
        {
            m_displayScoreType = static_cast<ScoreType>(static_cast<int>(m_displayScoreType) + 1);
        }
        //  全てのスコアの表示が終わったら、フラグを変更
        else
        {
            m_isFinishDisplayScore = true;
        }
    }
}

//  文字描画
void ResultScoreUI::DrawString()
{
    //  生存時間のスコア表示
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::ALIVE_STRING, ResultScoreUIData::ALIVE_TIME_SCORE_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::COLON_STRING, ResultScoreUIData::ALIVE_CORON_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore[ScoreType::AliveTime], ResultScoreUIData::ALIVE_TIME_SCORE_NUM_POS, ResultScoreUIData::SCORE_NUM_RATE);

    //  回避回数のスコア表示
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::DODGE_STRING, ResultScoreUIData::DODGE_SCORE_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::COLON_STRING, ResultScoreUIData::DODGE_CORON_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore[ScoreType::DodgeNum ], ResultScoreUIData::DODGE_SCORE_NUM_POS, ResultScoreUIData::SCORE_NUM_RATE);

    //  残機のスコア表示
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::LIFE_STRING, ResultScoreUIData::LIFE_SCORE_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::COLON_STRING, ResultScoreUIData::LIFE_CORON_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore[ScoreType::LifeNum], ResultScoreUIData::LIFE_SCORE_NUM_POS, ResultScoreUIData::SCORE_NUM_RATE);

    //  エネミーの撃破数のスコア表示
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::DEFEAT_STRING, ResultScoreUIData::DEFEAT_ENEMY_SCORE_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::COLON_STRING, ResultScoreUIData::DEFEAT_CORON_STRING_POS, ResultScoreUIData::SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore[ScoreType::DefeatEnemyNum], ResultScoreUIData::DEFEAT_ENEMY_NUM_SCORE_POS, ResultScoreUIData::SCORE_NUM_RATE);

    //  総スコア表示
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::TOTAL_SCORE_STRING, ResultScoreUIData::TOTAL_SCORE_STRING_POS, ResultScoreUIData::TOTAL_SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellow(ResultScoreUIData::COLON_STRING, ResultScoreUIData::TOTAL_SCORE_CORON_STRING_POS, ResultScoreUIData::TOTAL_SCORE_STRING_RATE);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore[ScoreType::Total], ResultScoreUIData::TOTAL_SCORE_NUM_POS, ResultScoreUIData::TOTAL_SCORE_NUM_RATE);
}

//  スコアのアニメーション
void ResultScoreUI::PlayAnimationScore(const ScoreType _scoreType)
{
    //  スコア表示を割合増加
    m_displayScore[_scoreType] += static_cast<int>(m_actualScore[_scoreType] * ResultScoreUIData::SCORE_INCREASE_RATE);
    //  スコアの増加SEを鳴らす
    m_sound.Play(SoundType::ScoreSE, false, true);
    //  表示されているスコアが、実際のスコアを超えないように調整
    if (m_displayScore[_scoreType] >= m_actualScore[_scoreType])
    {
        m_displayScore[_scoreType] = m_actualScore[_scoreType];
    }
}
