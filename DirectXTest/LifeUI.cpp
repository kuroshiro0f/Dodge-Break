#include "LifeUI.h"

#include "SpriteType.h"
#include "Timer.h"
#include "RestartData.h"

#include "IPlayerParameterGetter.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

//  一個目の残機の座標
static const XMFLOAT2 FIRST_LIFE_POS = { 45.0f,36.0f };

//  残機の画像の拡大率
static const float LIFE_SPRITE_MAGNIFICATION = 1.2f;
//  残機の画像同士の間隔
static const float LIFE_SPRITE_INTERVAL = 80;

//  残機のアニメーションにおける点滅の間隔
static const float FLASHING_INTERVAL = 10.0f;

LifeUI::LifeUI()
    : m_playerParameterGetter(nullptr)
    , m_sprite(Singleton<SpriteDrawer>::GetInstance())
    , m_flashingTimer(new Timer())
    , m_PlayAnimationTimer(new Timer())
    , m_displayLifeNum(0)
    , m_isPlayAnimation(false)
{
    //  処理なし
}

LifeUI::~LifeUI()
{
    delete m_flashingTimer;
    delete m_PlayAnimationTimer;
}

//  初期化
void LifeUI::Init(IPlayerParameterGetter* _playerParameterGetter)
{
    m_playerParameterGetter = _playerParameterGetter;
}

//  リセット
void LifeUI::Reset()
{
    //  時間計測クラスのリセット
    m_flashingTimer->Reset();
    //  表示する残機量を取得
    m_displayLifeNum = m_playerParameterGetter->GetLife();
    //  アニメーションはまだ始まっていない状態から
    m_isPlayAnimation = false;
}

void LifeUI::Update()
{
    //  残機量の更新
    if (m_displayLifeNum > m_playerParameterGetter->GetLife() && !m_isPlayAnimation)
    {
        //  アニメーション開始
        m_isPlayAnimation = true;
        //  点滅間隔計測開始
        m_flashingTimer->Start();
        //  アニメーション時間計測開始
        m_PlayAnimationTimer->Start();
    }
    if (m_isPlayAnimation)
    {
        //  残機の減少アニメーションを行う
        PlayAnimationDecreaseLife();
    }
}

//  描画
void LifeUI::Draw()
{
    //  一個目の残機の座標
    float firstLifePosX = FIRST_LIFE_POS.x;
    float firstLifePosY = FIRST_LIFE_POS.y;

    //  残機の画像を決定して描画
    for (int i = 0; i < m_displayLifeNum; i++)
    {
        m_sprite.Draw(SpriteType::Life, { firstLifePosX + LIFE_SPRITE_INTERVAL * i,firstLifePosY }, LIFE_SPRITE_MAGNIFICATION);
    }
}

//  残機の減少アニメーション
void LifeUI::PlayAnimationDecreaseLife()
{
    //  一定時間経過し、減少予定の残機が表示中なら、表示を消す
    if (m_flashingTimer->GetElapseTime() >= FLASHING_INTERVAL && m_displayLifeNum > m_playerParameterGetter->GetLife())
    {
        m_displayLifeNum--;
        //  プレイヤーの再登場までアニメーションしたら、アニメーション終了
        if (m_PlayAnimationTimer->GetElapseTime() >= RestartData::RESTART_TIME)
        {
            m_isPlayAnimation = false;
        }
        m_flashingTimer->Start();
    }
    //  一定時間経過し、減少予定の残機が非表示なら、表示する
    else if (m_flashingTimer->GetElapseTime() >= FLASHING_INTERVAL && m_displayLifeNum == m_playerParameterGetter->GetLife())
    {
        m_displayLifeNum++;
        m_flashingTimer->Start();
    }
}
