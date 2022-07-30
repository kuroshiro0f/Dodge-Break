#include "Result.h"

#include "SoundType.h"
#include "StringDrawer.h"
#include "SceneParam.h"
#include "SceneType.h"
#include "Mouse.h"
#include "Timer.h"
#include "ResultUI.h"

#include "Singleton.h"
#include "KeyBoard.h"
#include "SceneController.h"
#include "Sound.h"

Result::Result()
    :m_sound(Singleton<Sound>::GetInstance())
    , m_keyboard(Singleton<KeyBoard>::GetInstance())
    , m_sceneController(Singleton<SceneController>::GetInstance())
    , m_ui(new ResultUI())
    , m_mouse(new Mouse())
    , m_string(new StringDrawer())
    , m_waitTimer(new Timer())
    , m_bgmHandle(0)
    , m_isClick(false)
{
    //  処理なし
}

Result::~Result()
{
    //  メモリの解放
    delete m_ui;
    delete m_mouse;
    delete m_string;
    delete m_waitTimer;
}

//  更新
void Result::Update()
{
    //  UIの更新
    m_ui->Update(m_isClick);

    //  クリックされたら
    if (m_isClick)
    {
        //  一定時間待機したら
        if (m_waitTimer->GetElapseTime() >= SceneParam::TRANSITION_TIME)
        {
            //  BGMを停止
            m_sound.Stop(SoundType::ResultBGM,m_bgmHandle);
            //  タイトルへ移行
            m_sceneController.ChangeScene(SceneType::Title);
        }
    }
    else
    {
        //  決定ボタンが押されたら
        if (m_keyboard.IsPressKey(m_keyboard.KeyBind::Select))
        {
            //  SEを再生
            m_sound.Play(SoundType::TransitionTitleSE, false, false);
            //  クリックされた
            m_isClick = true;
            //  時間計測の開始
            m_waitTimer->Start();
           
        }
    }
}

//  画像描画
void Result::DrawSprite()
{
    //  UIの描画
    m_ui->DrawSprite();
}
//  文字描画
void Result::DrawString()
{
    //  UIの描画
    m_ui->DrawString();
}

//  BGMの再生開始
void Result::StartBGM()
{
    //  BGMを再生
    m_bgmHandle = m_sound.Play(SoundType::ResultBGM, true, true);
}

//  リセット
void Result::Reset()
{
    //  クリックはまだされていない
    m_isClick = false;
    m_ui->Reset();
    //  時間計測クラスのリセット
    m_waitTimer->Reset();
}
//  設定
void Result::SetUp(IScoreGetter* _score)
{
    //  UIの設定
    m_ui->SetUp(_score);
}
