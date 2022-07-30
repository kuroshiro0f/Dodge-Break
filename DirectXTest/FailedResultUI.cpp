#include "FailedResultUI.h"

#include <DirectXMath.h>

#include "ColorType.h"
#include "ColorValue.h"
#include "SpriteType.h"
#include "Timer.h"
#include "StringDrawer.h"
#include "ResultScoreUI.h"
#include "FailedResultUIData.h"
#include "TipsUI.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

using namespace DirectX;

FailedResultUI::FailedResultUI()
    :m_alfa(FailedResultUIData::ALFA_MIN)
    , m_color(new ColorValue())
    , m_scoreUI(new ResultScoreUI())
    , m_tipsUI(new TipsUI())
    , m_displayStateTimer(new Timer())
    , m_string(new StringDrawer())
    , m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_displayState(DisplayState::FadeOut)
{
    //  処理なし
}

FailedResultUI::~FailedResultUI()
{
    delete m_color;
    delete m_string;
    delete m_scoreUI;
    delete m_tipsUI;
    delete m_displayStateTimer;
}

//  リセット
void FailedResultUI::Reset()
{
    //  アルファ値のリセット
    m_alfa = FailedResultUIData::ALFA_MIN;
    //  フェードアウトから開始
    m_displayState = DisplayState::FadeOut;
    //  スコアUIのリセット
    m_scoreUI->Reset();
}

//  設定
void FailedResultUI::SetUp(IScoreGetter* _score)
{
    m_scoreUI->SetUp(_score);
}

//  更新
void FailedResultUI::Update()
{
    switch (m_displayState)
    {
    case FailedResultUI::DisplayState::FadeOut:
        //  フェードアウトを行う
        FadeOut();
        //  フェードアウトが終了したら文字の表示を開始
        if (m_alfa >= FailedResultUIData::ALFA_MAX)
        {
            m_displayState = DisplayState::AppearString;
            m_displayStateTimer->Start();
        }
        break;
    case FailedResultUI::DisplayState::AppearString:
        //  一定時間後にスコア表示開始
        if (m_displayStateTimer->GetElapseTime() >= FailedResultUIData::TRANGITION_APPEAR_STRING_FOR_SCORE_TIME)
        {
            m_displayState = DisplayState::Score;
            m_displayStateTimer->Stop();
        }
        break;
    case FailedResultUI::DisplayState::Score:
        //  スコアのUIの更新
        m_scoreUI->Update();
        if (m_scoreUI->IsFinishDisplayScore())
        {
            m_displayState = DisplayState::Tips;
            m_tipsUI->DecideDisplayTips();
            m_displayStateTimer->Start();
        }
        break;
    default:
        break;
    }
}

//  画像描画
void FailedResultUI::DrawSprite()
{
    m_sprite.Draw(SpriteType::FadeOutBack, FailedResultUIData::BACK_SPRITE_POS, FailedResultUIData::BACK_SPRITE_MAGNIFICATION, m_color->GetColorWithAlfa(ColorType::DefaultSpriteColor, m_alfa));
    if (static_cast<int>(m_displayState) >= static_cast<int>(DisplayState::Tips) 
        && m_displayStateTimer->GetElapseTime() >= FailedResultUIData::TRANGITION_SCORE_FOR_TIPS_TIME)
    {
        m_tipsUI->DrawSprite();
    }
}

//  文字描画
void FailedResultUI::DrawString()
{
    if (static_cast<int>(m_displayState) >= static_cast<int>(DisplayState::AppearString))
    {
        m_string->DrawStringBlackAndYellow(FailedResultUIData::RESULT_STRING, FailedResultUIData::RESULT_STRING_POS, FailedResultUIData::RESULT_STRING_RATE);
        m_scoreUI->DrawString();
    }
}

//  フェードアウト
void FailedResultUI::FadeOut()
{
    m_alfa += FailedResultUIData::INCREASE_ALFA_SPEED;
    if (m_alfa >= FailedResultUIData::ALFA_MAX)
    {
        m_alfa = FailedResultUIData::ALFA_MAX;
    }
}
