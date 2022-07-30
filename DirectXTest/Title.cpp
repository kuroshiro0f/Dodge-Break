#include "Title.h"

#include "SoundType.h"
#include "StringDrawer.h"
#include "SceneParam.h"
#include "SceneType.h"
#include "Mouse.h"
#include "Timer.h"
#include "TitleUI.h"

#include "Singleton.h"
#include "SceneController.h"
#include "Sound.h"
#include "KeyBoard.h"

//  ゲーム開始直後の待機時間
//  NOTE: 現在、時間の計算時の設定倍率を間違えているため、
//        こちらは0.5秒という意味です。
//  TODO: 時間の倍率を修正後に、値を調整
const float START_WAIT_TIME = 50.0f;

Title::Title()
    :m_sound(Singleton<Sound>::GetInstance())
    , m_keyboard(Singleton<KeyBoard>::GetInstance())
    , m_sceneController(Singleton<SceneController>::GetInstance())
    , m_ui(new TitleUI())
    , m_mouse(new Mouse())
    , m_string(new StringDrawer())
    , m_waitTimer(new Timer())
    , m_bgmHandle(0)
    , m_isClick(false)
{
}

Title::~Title()
{
    //  メモリの解放
    delete m_ui;
    delete m_mouse;
    delete m_string;
    delete m_waitTimer;
}

//  リセット
void Title::Reset()
{
    //  クリックはまだされていない
    m_isClick = false;
    //  時間計測クラスのリセット
    m_waitTimer->Reset();
}

//  更新
void Title::Update()
{
    //  UIの更新
    m_ui->Update(m_isClick);

    if (m_isClick)
    {
        //  一定時間待機したら
        if (m_waitTimer->GetElapseTime() >= SceneParam::TRANSITION_TIME)
        {
            //  BGMを停止
            m_sound.Stop(SoundType::TitleBGM,m_bgmHandle);
            //  ゲームシーンへ移行
            m_sceneController.ChangeScene(SceneType::GameScene);
        }
    }
    else
    {
        //  ゲーム開始後少し待つ
        //  NOTE: ゲーム開始直後からキー入力を受け付けると、
        //        exeファイルをダブルクリックしたときの判定が残ったままになって
        //        ゲームがすぐ始まってしまうため、少し待つ
        if (!m_waitTimer->IsStart())
        {
            m_waitTimer->Start();
        }
        //  決定ボタンが押されたら
        if (m_keyboard.IsPressKey(m_keyboard.KeyBind::Select) && m_waitTimer->GetElapseTime() >= START_WAIT_TIME)
        {
            //  SEを再生
            m_sound.Play(SoundType::TransitionGameSceneSE, false, false);
            //  クリックされた
            m_isClick = true;
            //  時間計測の開始
            m_waitTimer->Start();
        }
    }
}

//  描画
void Title::Draw()
{
    //  UIの描画
    m_ui->DrawSprite();
}

//  文字の描画
void Title::DrawString()
{
/*#i*//*f _DEBUG
    m_string->DrawStringBlackAndYellowForFewNumber(m_mouse->GetMouseScreenPos().x, { 0,0 });
    m_string->DrawStringBlackAndYellowForFewNumber(m_mouse->GetMouseScreenPos().y, { 0,100 });
#endif*/
}

//  BGMの再生開始
void Title::StartBGM()
{
    m_bgmHandle = m_sound.Play(SoundType::TitleBGM, true, true);
}
