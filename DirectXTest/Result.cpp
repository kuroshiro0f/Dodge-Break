#include "Result.h"

#include "SoundType.h"
#include "StringDrawer.h"
#include "SceneParam.h"
#include "SceneType.h"
#include "Mouse.h"
#include "Timer.h"
#include "ResultUI.h"

#include "Singleton.h"
#include "SceneController.h"
#include "Sound.h"
#include "UserInputHandler.h"

Result::Result()
    :m_sound(Singleton<Sound>::GetInstance())
    , m_sceneController(Singleton<SceneController>::GetInstance())
    , m_ui(new ResultUI())
    , m_mouse(new Mouse())
    , m_string(new StringDrawer())
    , m_waitTimer(new Timer())
    , m_bgmHandle(0)
    , m_state(ResultState::None)
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
    m_ui->Update(m_state != ResultState::Play);

    //  ボタンが押されたら
    if (m_state == ResultState::PressButton)
    {
        //  SEを再生
        m_sound.Play(SoundType::TransitionTitleSE, false, false);
        //  シーン移行の準備開始
        m_state = ResultState::ReadyToChangeScene;
        //  時間計測の開始
        m_waitTimer->Start();

    }
    //  ボタンが押された後、一定時間待機したら
    if (m_state == ResultState::ReadyToChangeScene)
    {
        //  BGMを停止
        m_sound.Stop(SoundType::ResultBGM, m_bgmHandle);
        //  タイトルへ移行
        m_sceneController.ChangeScene(SceneType::Title);
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

//  次へ進む
void Result::ToNext()
{
    if (m_state == ResultState::Play)
    {
        m_state = ResultState::PressButton;
    }
}

//  リセット
void Result::Reset()
{
    //  リザルトシーンの再生から
    m_state = ResultState::Play;
    m_ui->Reset();

    //  入力検知のクラスのシングルトンインスタンスを取得
    UserInputHandler& userInputHandler = Singleton<UserInputHandler>::GetInstance();
    //  入力検知のリセット
    userInputHandler.ResetOperationFunc();
    //  入力検知時の命令を登録
    userInputHandler.RegisterOperation(UserInputHandler::OperationType::ToNext, std::bind(&Result::ToNext, this));

    //  時間計測クラスのリセット
    m_waitTimer->Reset();
}
//  設定
void Result::SetUp(IScoreGetter* _score)
{
    //  UIの設定
    m_ui->SetUp(_score);
}
