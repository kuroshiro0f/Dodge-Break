#include "GameSceneScoreUI.h"

#include "ScoreType.h"
#include "StringDrawer.h"

#include <DirectXMath.h>

#include "IScoreGetter.h"

using namespace DirectX;

//  スコアの増加量
static const float SCORE_INCREASE_RATE = 0.05;

//  スコア文字列の表示座標
static const XMFLOAT2 SCORE_STRING_POS = { 1500,26 };
//  スコアの表示座標
static const XMFLOAT2 SCORE_POS = { 1670,26 };
//  スコアの表示倍率
static const float SCORE_MAGNIFICATION = 0.7f;

GameSceneScoreUI::GameSceneScoreUI()
    :m_scoreGetter(nullptr)
    , m_displayScore(0)
    , m_actualScore(0)
    , m_startAnimationScore(0)
    , m_totalScore(0)
    , m_string(new StringDrawer())
{
    //  処理なし
}

GameSceneScoreUI::~GameSceneScoreUI()
{
    delete m_string;
}

//  初期化
void GameSceneScoreUI::Init(class IScoreGetter* _scoreGetter)
{
    m_scoreGetter = _scoreGetter;
}

//  リセット
void GameSceneScoreUI::Reset()
{
    //  各スコアのリセット
    m_displayScore = 0;
    m_actualScore = 0;
    m_startAnimationScore = 0;
    m_totalScore = 0;
}

//  更新
void GameSceneScoreUI::Update()
{
    m_totalScore = m_scoreGetter->GetScore(ScoreType::Total);

    if (m_actualScore < m_totalScore)
    {
        m_actualScore = m_totalScore;
        m_startAnimationScore = m_displayScore;
    }
    if (m_displayScore < m_actualScore)
    {
        PlayAnimationScoreIncrease();
    }
}

//  文字描画
void GameSceneScoreUI::DrawString()
{
    m_string->DrawStringBlackAndYellow("SCORE : ", SCORE_STRING_POS, SCORE_MAGNIFICATION);
    m_string->DrawStringBlackAndYellowForNumber(m_displayScore, SCORE_POS, SCORE_MAGNIFICATION);
}

//  スコアの増加アニメーション
void GameSceneScoreUI::PlayAnimationScoreIncrease()
{
    //  スコア表示を割合増加
    m_displayScore += static_cast<int>((m_actualScore - m_startAnimationScore) * SCORE_INCREASE_RATE);

    if (m_displayScore >= m_totalScore)
    {
        m_displayScore = m_totalScore;
    }
}
