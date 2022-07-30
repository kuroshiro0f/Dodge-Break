#include "ResultUI.h"

#include <DirectXMath.h>

#include "ColorType.h"
#include "ColorValue.h"
#include "SpriteType.h"
#include "Timer.h"
#include "StringDrawer.h"
#include "ResultUIData.h"
#include "ResultScoreUI.h"
#include "ScoreEvaluationUI.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

using namespace DirectX;

ResultUI::ResultUI()
    :m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_string(new StringDrawer())
    , m_color(new ColorValue())
    , m_scoreUI(new ResultScoreUI())
    , m_ScoreEvaluationUI(new ScoreEvaluationUI())
    , m_displayStateTimer(new Timer())
    , m_displayState(DisplayState::AppearScoreString)
    , m_isMakeTransparent(true)
    , m_alfaValue(ResultUIData::ALFA_VALUE_MAX)
{
    //  処理なし
}

ResultUI::~ResultUI()
{
    //  メモリの解放
    delete m_string;
    delete m_color;
    delete m_scoreUI;
    delete m_displayStateTimer;
}

//  リセット
void ResultUI::Reset()
{
    //  アルファ値のリセット
    m_alfaValue = ResultUIData::ALFA_VALUE_MAX;
    //  スコアの文字表示から
    m_displayState = DisplayState::AppearScoreString;
    //  スコアUIのリセット
    m_scoreUI->Reset();
    //  評価UIのリセット
    m_ScoreEvaluationUI->Reset();
}

//  設定
void ResultUI::SetUp(IScoreGetter* _score)
{
    m_scoreUI->SetUp(_score);
    m_ScoreEvaluationUI->SetUp(_score);
    m_displayStateTimer->Start();
}

//  更新
void ResultUI::Update(const bool _isClick)
{
    switch (m_displayState)
    {
    case ResultUI::DisplayState::AppearScoreString:
        //  一定時間後にスコア表示開始
        if (m_displayStateTimer->GetElapseTime() >= ResultUIData::TRANGITION_APPEAR_STRING_FOR_SCORE_TIME)
        {
            m_displayState = DisplayState::Score;
            m_displayStateTimer->Stop();
        }
        break;
    case ResultUI::DisplayState::Score:
        //  スコアのUIの更新
        m_scoreUI->Update();
        //  スコアの表示が終了したら評価を表示
        if (m_scoreUI->IsFinishDisplayScore())
        {
            m_displayState = DisplayState::Evaluate;
            m_displayStateTimer->Start();
        }
        break;
    case ResultUI::DisplayState::Evaluate:
        //  一定時間後に評価を表示
        if (m_displayStateTimer->GetElapseTime() >= ResultUIData::TRANGITION_SCORE_FOR_EVALUATE_TIME)
        {
            //  評価のUIの更新
            m_ScoreEvaluationUI->Update();
        }
        //  評価の表示が終了したらガイドを表示
        if (!m_ScoreEvaluationUI->IsPlayAnimation())
        {
            m_displayState = DisplayState::AppearGuideString;
        }
        break;
    case ResultUI::DisplayState::AppearGuideString:
        //  クリックの有無で処理を分岐
        if (!_isClick)
        {
            PlayAnimationFlashing(ResultUIData::FLASHING_SPEED_DEFAULT);
        }
        else
        {
            PlayAnimationFlashing(ResultUIData::FLASHING_SPEED_DEFAULT * ResultUIData::UP_FLASHING_SPEED_MAGNIFICATION);
        }
        break;
    default:
        break;
    }
}

//  画像描画
void ResultUI::DrawSprite()
{
    //  背景描画
    m_sprite.Draw(SpriteType::ResultBack, { 0,0 });
    if (static_cast<int>(m_displayState) >= static_cast<int>(DisplayState::Evaluate))
    {
        //  一定時間後に評価を表示
        if (m_displayStateTimer->GetElapseTime() >= ResultUIData::TRANGITION_SCORE_FOR_EVALUATE_TIME)
        {
            m_ScoreEvaluationUI->DrawSprite();
        }
    }
    if (static_cast<int>(m_displayState) >= static_cast<int>(DisplayState::AppearGuideString))
    {
        //  タイトルへの誘導文字の描画
        m_sprite.Draw(SpriteType::Return, { 0,0 }, 1.0f, m_color->GetColorWithAlfa(ColorType::DefaultSpriteColor, m_alfaValue));
    }
}

//  文字描画
void ResultUI::DrawString()
{
    if (static_cast<int>(m_displayState) >= static_cast<int>(DisplayState::AppearScoreString))
    {
        m_string->DrawStringBlackAndYellow(ResultUIData::RESULT_STRING, ResultUIData::RESULT_STRING_POS, ResultUIData::RESULT_STRING_RATE);
        m_scoreUI->DrawString();
    }
}

//  点滅アニメーション
void ResultUI::PlayAnimationFlashing(const float _flashingSpeed)
{
    //  文字の透過度を更新し、アニメーションを行う
    if (m_isMakeTransparent)
    {
        m_alfaValue += _flashingSpeed;
        if (m_alfaValue >= ResultUIData::ALFA_VALUE_MAX)
        {
            m_alfaValue = ResultUIData::ALFA_VALUE_MAX;
            m_isMakeTransparent = false;
        }
    }
    else if (!m_isMakeTransparent)
    {
        m_alfaValue -= _flashingSpeed;
        if (m_alfaValue <= ResultUIData::ALFA_VALUE_MIN)
        {
            m_alfaValue = ResultUIData::ALFA_VALUE_MIN;
            m_isMakeTransparent = true;
        }
    }
}