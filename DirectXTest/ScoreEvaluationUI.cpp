#include "ScoreEvaluationUI.h"

#include "StringDrawer.h"
#include "SpriteType.h"
#include "SoundType.h"
#include "ScoreType.h"
#include "ColorType.h"
#include "ColorValue.h"
#include "IScoreGetter.h"
#include "Timer.h"
#include "ScoreEvaluationUIData.h"

#include "Singleton.h"
#include "SpriteDrawer.h"
#include "Sound.h"

ScoreEvaluationUI::ScoreEvaluationUI()
    :m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_sound(Singleton<Sound>::GetInstance())
    , m_displayTimer(new Timer())
    , m_color(new ColorValue())
    , m_magnification(ScoreEvaluationUIData::MAGNIFICATION_MAX)
    , m_alfa(ScoreEvaluationUIData::ALFA_MIN)
    , m_moveRate(0)
    , m_maxMoveRate(0)
    , m_pos{ 0,0 }
    , m_isPlayAnimation(true)
{
    //  処理なし
}

ScoreEvaluationUI::~ScoreEvaluationUI()
{
    delete m_displayTimer;
    delete m_color;
}

//  リセット
void ScoreEvaluationUI::Reset()
{
    //  拡大率のリセット
    m_magnification = ScoreEvaluationUIData::MAGNIFICATION_MAX;
    //  アルファ値のリセット
    m_alfa = ScoreEvaluationUIData::ALFA_MIN;
    //  座標のリセット
    m_pos = { 0,0 };
    //  移動割合のリセット
    m_moveRate = 0;
    //  最大移動割合のリセット
    m_maxMoveRate = 0;
    //  アニメーションは初めからする
    m_isPlayAnimation = true;
}

//  設定
void ScoreEvaluationUI::SetUp(IScoreGetter* _score)
{
    DecideEvaluate(_score->GetScore(ScoreType::Total));
}

//  更新
void ScoreEvaluationUI::Update()
{
    if (m_isPlayAnimation)
    {
        PlayAnimation();
    }
}

//  画像描画
void ScoreEvaluationUI::DrawSprite()
{
    float posX = m_pos.x;
    m_sprite.Draw(m_spriteType, { posX - (posX * m_moveRate),m_pos.y }, m_magnification, m_color->GetColorWithAlfa(ColorType::DefaultSpriteColor, m_alfa));
}

//  評価の決定
void ScoreEvaluationUI::DecideEvaluate(const int _totalScore)
{
    //  総スコアによって評価段階を決定
    //  1段階目の評価
    if (_totalScore >= ScoreEvaluationUIData::FIRST_EVALUATE_SCORE && _totalScore < ScoreEvaluationUIData::SECOND_EVALUATE_SCORE)
    {
        m_spriteType = ScoreEvaluationUIData::FIRST_EVALUATE_SPRITE_TYPE;
        m_pos = ScoreEvaluationUIData::FIRST_EVALUATE_POS;
        m_moveRate = ScoreEvaluationUIData::FIRST_POS_MOVE_RATE_MAX;
        m_maxMoveRate = ScoreEvaluationUIData::FIRST_POS_MOVE_RATE_MAX;
    }
    //  2段階目の評価
    else if (_totalScore >= ScoreEvaluationUIData::SECOND_EVALUATE_SCORE && _totalScore < ScoreEvaluationUIData::THIRD_EVALUATE_SCORE)
    {
        m_spriteType = ScoreEvaluationUIData::SECOND_EVALUATE_SPRITE_TYPE;
        m_pos = ScoreEvaluationUIData::SECOND_EVALUATE_POS;
        m_moveRate = ScoreEvaluationUIData::SECOND_POS_MOVE_RATE_MAX;
        m_maxMoveRate = ScoreEvaluationUIData::SECOND_POS_MOVE_RATE_MAX;
    }
    //  3段階目の評価
    else if (_totalScore >= ScoreEvaluationUIData::THIRD_EVALUATE_SCORE && _totalScore < ScoreEvaluationUIData::FORTH_EVALUATE_SCORE)
    {
        m_spriteType = ScoreEvaluationUIData::THIRD_EVALUATE_SPRITE_TYPE;
        m_pos = ScoreEvaluationUIData::THIRD_EVALUATE_POS;
        m_moveRate = ScoreEvaluationUIData::THIRD_POS_MOVE_RATE_MAX;
        m_maxMoveRate = ScoreEvaluationUIData::THIRD_POS_MOVE_RATE_MAX;
    }
    //  4段階目の評価
    else if (_totalScore >= ScoreEvaluationUIData::FORTH_EVALUATE_SCORE)
    {
        m_spriteType = ScoreEvaluationUIData::FORTH_EVALUATE_SPRITE_TYPE;
        m_pos = ScoreEvaluationUIData::FORTH_EVALUATE_POS;
        m_moveRate = ScoreEvaluationUIData::FORTH_POS_MOVE_RATE_MAX;
        m_maxMoveRate = ScoreEvaluationUIData::FORTH_POS_MOVE_RATE_MAX;
    }
}

//  アニメーション
void ScoreEvaluationUI::PlayAnimation()
{
    //  第一段階のアニメーション
    if (m_magnification >= ScoreEvaluationUIData::ANIMATION_SPEED_CHANGE_POINT)
    {
        m_magnification -= (ScoreEvaluationUIData::MAGNIFICATION_MAX - ScoreEvaluationUIData::MAGNIFICATION_MIN) * ScoreEvaluationUIData::FIRST_PLAY_ANIMATION_SPEED;
        m_alfa += (ScoreEvaluationUIData::ALFA_MAX - ScoreEvaluationUIData::ALFA_MIN) * ScoreEvaluationUIData::FIRST_PLAY_ANIMATION_SPEED;
        m_moveRate -= (m_maxMoveRate - ScoreEvaluationUIData::POS_MOVE_RATE_MIN) * ScoreEvaluationUIData::FIRST_PLAY_ANIMATION_SPEED;
    }
    //  第二段階のアニメーション
    else
    {
        m_magnification -= (ScoreEvaluationUIData::MAGNIFICATION_MAX - ScoreEvaluationUIData::MAGNIFICATION_MIN) * ScoreEvaluationUIData::SECOND_PLAY_ANIMATION_SPEED;
        m_alfa += (ScoreEvaluationUIData::ALFA_MAX - ScoreEvaluationUIData::ALFA_MIN) * ScoreEvaluationUIData::SECOND_PLAY_ANIMATION_SPEED;
        m_moveRate -= (m_maxMoveRate - ScoreEvaluationUIData::POS_MOVE_RATE_MIN) * ScoreEvaluationUIData::SECOND_PLAY_ANIMATION_SPEED;
    }

    //  拡大率の変動が終了したか確認
    if (m_magnification <= ScoreEvaluationUIData::MAGNIFICATION_MIN)
    {
        m_magnification = ScoreEvaluationUIData::MAGNIFICATION_MIN;
    }
    //  アルファ値の変動が終了したか確認
    if (m_alfa >= ScoreEvaluationUIData::ALFA_MAX)
    {
        m_alfa = ScoreEvaluationUIData::ALFA_MAX;
    }
    //  移動割合の変動が終了したか確認
    if (m_moveRate <= ScoreEvaluationUIData::POS_MOVE_RATE_MIN)
    {
        m_moveRate = ScoreEvaluationUIData::POS_MOVE_RATE_MIN;
    }

    //  アニメーションにて変動する値が全て変動し終わったら、サウンドを鳴らしてアニメーション終了
    if (m_magnification <= ScoreEvaluationUIData::MAGNIFICATION_MIN && m_alfa >= ScoreEvaluationUIData::ALFA_MAX && m_moveRate <= ScoreEvaluationUIData::POS_MOVE_RATE_MIN)
    {
        m_sound.Play(SoundType::ValueSE, false, true);
        m_isPlayAnimation = false;
    }
}
