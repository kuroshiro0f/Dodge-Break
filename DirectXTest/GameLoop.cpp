#include "GameLoop.h"

#include "StringDrawer.h"
#include "GameScene.h"
#include "Result.h"
#include "Title.h"
#include "Dx12Wrapper.h"
#include "PMDRenderer.h"
#include "Timer.h"

#include "Singleton.h"
#include "CheckError.h"
#include "Device.h"
#include "PMDModel.h"
#include "SpriteDrawer.h"
#include "Sound.h"
#include "LoadJson.h"
#include "EffekseerManager.h"
#include "SceneController.h"
#include "SingletonFinalizer.h"
#include "EnemyAttackPool.h"
#include "UserInputHandler.h"

GameLoop::GameLoop()
    :m_device(Singleton<Device>::GetInstance())
    , m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_userInputHandler(Singleton<UserInputHandler>::GetInstance())
    , m_error(Singleton<CheckError>::GetInstance())
    , m_deltaTime(0)
    , m_string(nullptr)
    , m_gameLoopTimer(nullptr)
    , m_isFinishRun(false)
{
}

GameLoop::~GameLoop()
{
    //  メモリの解放
    delete m_string;
    delete m_gameLoopTimer;
}

//  初期化
void GameLoop::Init()
{
    //  デバイスの初期化
    if (!m_device.Init()) {
        return;
    }
    ShowWindow(m_device.hwnd, SW_SHOW);    //  ウィンドウ表示

    //  各クラスのインスタンスを生成
    m_string = new StringDrawer();    //  文字描画
    m_gameLoopTimer = new Timer();      //  時間計測

    //  Effekseer関連初期化
    m_effect.Init();

    //  入力処理の初期化
    m_userInputHandler.Init();
    //  ループ終了用の関数を登録
    m_userInputHandler.RegisterOperation(UserInputHandler::OperationType::FinishGame, std::bind(&GameLoop::FinishRun, this));

    //  PMDモデルクラスのシングルトンインスタンス生成
    PMDModel& model = Singleton<PMDModel>::GetInstance();
    model.Init();

    SpriteDrawer& spriteDrawer = Singleton<SpriteDrawer>::GetInstance();
    spriteDrawer.Init();

    Sound& sound = Singleton<Sound>::GetInstance();
    sound.Init();

    LoadJson& loadJson = Singleton<LoadJson>::GetInstance();
    loadJson.Init();

    EnemyAttackPool& attackPool = Singleton<EnemyAttackPool>::GetInstance();
    attackPool.Init();

    //  シーンの管理クラスを設定
    SceneController& sceneController = Singleton<SceneController>::GetInstance();
    sceneController.Init();

    //  最初の経過時間を仮に0.016に設定
    //  NOTE: 最初のループ前では経過時間を計算できないので、仮の値をセットしています
    m_deltaTime = 0.016f;
}

//  ループ
void GameLoop::Run()
{
    //  メッセージ格納変数
    MSG msg = {};
    
    //  PMDモデルクラスのシングルトンインスタンス取得
    PMDModel& model = Singleton<PMDModel>::GetInstance();

    //  シーンの管理クラスを設定
    SceneController& sceneController = Singleton<SceneController>::GetInstance();

    //  時間計測の開始
    m_gameLoopTimer->Start();

    //  ESCキーが押されるまでループ
    while (!m_isFinishRun) 
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //  もうアプリケーションが終わるって時にmessageがWM_QUITになる
        if (msg.message == WM_QUIT) {
            break;
        }

        //  エラーが発生したらループを抜ける
        if (m_error.IsOccurError())
        {
            return;
        }

        //  入力の更新
        m_userInputHandler.UpdateInput();
    
        //  全体の描画準備
        m_device.dx12->BeginDraw();

        //  PipelineStateをPMD用に合わせる
        m_device.dx12->GetCommandList()->SetPipelineState(m_device.pmdRenderer->GetPipelineState());
        //  RootSignatureもPMD用に合わせる
        m_device.dx12->GetCommandList()->SetGraphicsRootSignature(m_device.pmdRenderer->GetRootSignature());
        m_device.dx12->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_device.dx12->SetScene();

        //  シーンの更新
        sceneController.Update(m_deltaTime);

        //  描画の終了
        m_device.dx12->EndDraw();

        //  デルタタイムの更新
        m_deltaTime = m_gameLoopTimer->GetElapseTime();
        //  時間計測の開始
        m_gameLoopTimer->Start();

        //  フリップ
        m_device.dx12->GetSwapchain()->Present(1, 0);
        m_device.gmemory->Commit(m_device.dx12->GetCmdQue());
    }
    //  ゲーム終了処理
    m_device.Finalize();
    SingletonFinalizer::Finalize();
}

//  ループの終了
void GameLoop::FinishRun()
{
    m_isFinishRun = true;
}
