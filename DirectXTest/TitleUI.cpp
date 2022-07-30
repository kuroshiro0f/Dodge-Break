#include "TitleUI.h"

#include "ColorType.h"
#include "ColorValue.h"
#include "SpriteType.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

//  点滅速度
static const float FLASHING_SPEED_DEFAULT = 0.01f;
//  加速時の点滅速度の加速率
static const float UP_FLASHING_SPEED_MAGNIFICATION = 4.0f;
//  点滅時のアルファ値の最大値と最小値
static const float ALFA_VALUE_MAX = 1.0f;
static const float ALFA_VALUE_MIN = 0.1f;

TitleUI::TitleUI()
    :m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_color(new ColorValue())
    , m_isMakeTransparent(true)
    , m_alfaValue(1.0f)
{
    //  処理なし
}

TitleUI::~TitleUI()
{
    delete m_color;
}

//  更新
void TitleUI::Update(const bool _isClick)
{
    //  クリックの有無で処理を分岐
    if (!_isClick)
    {
        PlayAnimationFlashing(FLASHING_SPEED_DEFAULT);
    }
    else
    {
        PlayAnimationFlashing(FLASHING_SPEED_DEFAULT * UP_FLASHING_SPEED_MAGNIFICATION);
    }
}

//  画像描画
void TitleUI::DrawSprite()
{
    //  NOTE: 画面のサイズに合わせて作っているので座標は0
    XMFLOAT2 pos = { 0,0 };
    //  背景描画
    m_sprite.Draw(SpriteType::TitleBack, pos);
    //  画像で作成したタイトル文字の描画
    m_sprite.Draw(SpriteType::TitleText, pos);
    //  画像で作成したGameSceneへの案内をする文字の描画
    m_sprite.Draw(SpriteType::Start, pos, 1.0f, m_color->GetColorWithAlfa(ColorType::DefaultSpriteColor, m_alfaValue));
}

//  点滅アニメーション
void TitleUI::PlayAnimationFlashing(const float _flashingSpeed)
{
    //  文字の透過度を更新し、アニメーションを行う
    if (m_isMakeTransparent)
    {
        m_alfaValue += _flashingSpeed;
        if (m_alfaValue >= ALFA_VALUE_MAX)
        {
            m_alfaValue = ALFA_VALUE_MAX;
            m_isMakeTransparent = false;
        }
    }
    else if (!m_isMakeTransparent)
    {
        m_alfaValue -= _flashingSpeed;
        if (m_alfaValue <= ALFA_VALUE_MIN)
        {
            m_alfaValue = ALFA_VALUE_MIN;
            m_isMakeTransparent = true;
        }
    }
}
