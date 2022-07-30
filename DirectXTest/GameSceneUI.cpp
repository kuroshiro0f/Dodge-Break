#include "GameSceneUI.h"

#include <DirectXMath.h>

#include "SpriteType.h"
#include "ColorValue.h"
#include "ColorType.h"
#include "EnergyGaugeUI.h"
#include "LifeUI.h"
#include "GameSceneScoreUI.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

using namespace DirectX;

//  アルファ値関連
//  フェードアウトで使用
//  最大値
static const float ALFA_MAX = 1.0f;
//  最小値
static const float ALFA_MIN = 0;
//  アルファ値の上昇速度
static const float INCREASE_ALFA_SPEED = 0.03f;

//  フェードアウト用の画像
//  座標
static const XMFLOAT2 FADE_OUT_SPRITE_POS = { 0,0 };
//  拡大率
static const float FADE_OUT_SPRITE_MAGNIFICATION = 1.0f;

GameSceneUI::GameSceneUI()
    :m_energyGaugeUI(new EnergyGaugeUI())
    , m_lifeUI(new LifeUI())
    , m_scoreUI(new GameSceneScoreUI())
    , m_color(new ColorValue())
    , m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_alfa(ALFA_MIN)
    , m_isStartFadeOut(false)
{
    //  処理なし
}

GameSceneUI::~GameSceneUI()
{
    delete m_lifeUI;
    delete m_scoreUI;
    delete m_energyGaugeUI;
    delete m_color;
}

//  初期化
void GameSceneUI::Init(class IPlayerParameterGetter* _playerParameterGetter, class IScoreGetter* _scoreGetter, const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  エネルギーゲージUIクラスの初期化
    m_energyGaugeUI->Init(_playerParameterGetter, _eventClass);
    //  残機UIの初期化
    m_lifeUI->Init(_playerParameterGetter);
    //  スコアUIの初期化
    m_scoreUI->Init(_scoreGetter);
}

//  リセット
void GameSceneUI::Reset()
{
    //  エネルギーゲージUIクラスのリセット
    m_energyGaugeUI->Reset();
    //  残機UIクラスのリセット
    m_lifeUI->Reset();
    //  スコアUIクラスのリセット
    m_scoreUI->Reset();
    //  アルファ値のリセット
    m_alfa = ALFA_MIN;
    //  フェードアウトを開始前に
    m_isStartFadeOut = false;
}

//  更新
void GameSceneUI::Update()
{
    //  エネルギーゲージUIの更新
    m_energyGaugeUI->Update();
    //  残機UIの更新
    m_lifeUI->Update();
    //  スコアUIの更新
    m_scoreUI->Update();
}

//  画像描画
void GameSceneUI::DrawSprite()
{
    //  残機UIの描画
    m_lifeUI->Draw();
    //  エネルギーゲージUIの描画
    m_energyGaugeUI->Draw();
    //  フェードアウトが始まっていたら
    if (m_isStartFadeOut)
    {
        m_sprite.Draw(SpriteType::FadeOutBack, FADE_OUT_SPRITE_POS, FADE_OUT_SPRITE_MAGNIFICATION, m_color->GetColorWithAlfa(ColorType::DefaultSpriteColor, m_alfa));
    }
}

//  文字描画
void GameSceneUI::DrawString()
{
    //  スコアの表示
    m_scoreUI->DrawString();
}

//  フェードアウトの処理
void GameSceneUI::FadeOut()
{
    //  フェードアウト開始
    if (!m_isStartFadeOut)
    {
        m_isStartFadeOut = true;
    }

    //  アルファ値を増加
    m_alfa += INCREASE_ALFA_SPEED;
    //  アルファ値が最大値を超えないようにする
    if (m_alfa >= ALFA_MAX)
    {
        m_alfa = ALFA_MAX;
    }
}

//  フェードアウトは終わったか
bool GameSceneUI::IsFinishFadeOut()
{
    return m_alfa == ALFA_MAX;
}
