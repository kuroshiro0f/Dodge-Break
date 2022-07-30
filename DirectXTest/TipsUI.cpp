#include "TipsUI.h"

#include <DirectXMath.h>

#include <random>

#include "SpriteType.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

using namespace DirectX;

//  画像の情報
//  座標
static const XMFLOAT2 SPRITE_POS = { 500,880 };
//  拡大率
static const float SPRITE_MAGNIFICATION = 1.0f;

//  Tipsの数
static const int TIPS_NUM = 3;

TipsUI::TipsUI()
    :m_sprite(Singleton<SpriteDrawer>::GetInstance())
{
    //  処理なし
}

TipsUI::~TipsUI()
{
    //  処理なし
}

//  表示するTipsを決定
void TipsUI::DecideDisplayTips()
{
    //  乱数を生成
    std::random_device rand;
    std::mt19937_64 mt(rand());
    std::uniform_int_distribution<> randStageNum(1, TIPS_NUM);

    //  表示するTipsをランダムに決定
    switch (randStageNum(mt))
    {
    case 1:
        m_spriteType = SpriteType::Tips1;
        break;
    case 2:
        m_spriteType = SpriteType::Tips2;
        break;
    case 3:
        m_spriteType = SpriteType::Tips3;
        break;
    default:
        break;
    }
}

//  画像描画
void TipsUI::DrawSprite()
{
    m_sprite.Draw(m_spriteType, SPRITE_POS, SPRITE_MAGNIFICATION);
}
