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
#include "UserInputHandler.h"

//  ゲーム開始直後の待機時間
//  NOTE: 現在、時間の計算時の設定倍率を間違えているため、
//        こちらは0.5秒という意味です。
//  TODO: 時間の倍率を修正後に、値を調整
const float START_WAIT_TIME = 50.0f;

Title::Title()
    :m_sound(Singleton<Sound>::GetInstance())
    , m_sceneController(Singleton<SceneController>::GetInstance())
    , m_ui(new TitleUI())
    , m_mouse(new Mouse())
    , m_string(new StringDrawer())
    , m_waitTimer(new Timer())
    , m_bgmHandle(0)
    , m_state(TitleState::None)
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
    //  入力検知クラスのシングルトンインスタンス取得
    UserInputHandler& userInputHandler = Singleton<UserInputHandler>::GetInstance();

    //  入力検知をリセット
    userInputHandler.ResetOperationFunc();
    //  各関数を入力検知時の命令として登録
    userInputHandler.RegisterOperation(UserInputHandler::OperationType::ToNext, std::bind(&Title::ToNext, this));

    //  タイトル再生中から
    m_state = TitleState::Play;
    //  時間計測クラスのリセット
    m_waitTimer->Reset();
}

//  更新
void Title::Update()
{
    //  UIの更新
    m_ui->Update(m_state != TitleState::Play);

    //  ゲーム開始後少し待つ
    //  NOTE: ゲーム開始直後からキー入力を受け付けると、
    //        exeファイルをダブルクリックしたときの判定が残ったままになって
    //        ゲームがすぐ始まってしまうため、少し待つ
    if (!m_waitTimer->IsStart())
    {
        m_waitTimer->Start();
    }
    //  ボタンが押されたら
    else if (m_state == TitleState::PressButton)
    {
        //  SEを再生
        m_sound.Play(SoundType::TransitionGameSceneSE, false, false);
        //  シーン遷移処理の開始
        m_state = TitleState::ReadyToChangeScene;
        //  時間計測の開始
        m_waitTimer->Start();
    }
    //  ボタンが押された後、一定時間待機したら
    else if (m_state == TitleState::ReadyToChangeScene && m_waitTimer->GetElapseTime() >= SceneParam::TRANSITION_TIME)
    {
        //  BGMを停止
        m_sound.Stop(SoundType::TitleBGM, m_bgmHandle);
        //  ゲームシーンへ移行
        m_sceneController.ChangeScene(SceneType::GameScene);
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

//  次へ進む
void Title::ToNext()
{
    if (m_state == TitleState::Play && m_waitTimer->GetElapseTime() >= START_WAIT_TIME)
    {
        m_state = TitleState::PressButton;
    }
}
