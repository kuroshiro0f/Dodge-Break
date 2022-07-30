#include "SceneController.h"

#include "SceneType.h"
#include "Title.h"
#include "GameScene.h"
#include "Result.h"
#include "Dx12Wrapper.h"

#include "IScoreGetter.h"

#include "Singleton.h"
#include "Device.h"
#include "EffekseerManager.h"

SceneController::SceneController()
    :m_device(Singleton<Device>::GetInstance())
    , m_effect(Singleton<EffekseerManager>::GetInstance())
    , m_type(SceneType::Title)
    , m_title(nullptr)
    , m_gameScene(nullptr)
    , m_result(nullptr)
{
}

SceneController::~SceneController()
{
    //  メモリの解放
    delete m_title;
    delete m_gameScene;
    delete m_result;
}

//  初期化
void SceneController::Init()
{
    m_title = new Title();
    m_gameScene = new GameScene();
    m_result = new Result();

    //  ゲームシーンの初期化
    m_gameScene->Init();

    //  最初のシーンをセット
    ChangeScene(m_type);
}

//  更新
void SceneController::Update(const float _deltaTime)
{
    //  シーンの更新
    //  シーンごとに分岐
    switch (m_type)
    {
    case SceneType::Title:
        m_title->Update();
        break;
    case SceneType::GameScene:
        m_gameScene->Update(_deltaTime);
        break;
    case SceneType::Result:
        m_result->Update();
        break;
    default:
        break;
    }

    //  モデルの描画
    DrawModel();

    //  エフェクトの描画
    m_effect.SyncronizeEffekseerCamera();
    m_effect.Draw();

    //  画像描画
    DrawSprite();
    //  文字描画
    DrawString();
}

//  モデルの描画
void SceneController::DrawModel()
{
    //  シーンごとに分岐
    switch (m_type)
    {
    case SceneType::Title:
        break;
    case SceneType::GameScene:
        m_gameScene->DrawModel();
        break;
    case SceneType::Result:
        break;
    default:
        break;
    }
}

//  画像の描画
void SceneController::DrawSprite()
{
    //  シーンごとに分岐
    switch (m_type)
    {
    case SceneType::Title:
        m_title->Draw();
        break;
    case SceneType::GameScene:
        m_gameScene->DrawSprite();
        break;
    case SceneType::Result:
        m_result->DrawSprite();
        break;
    default:
        break;
    }
}

//  文字の描画
void SceneController::DrawString()
{
    //  文字の描画をするための前準備
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(1, m_device.heapForSpriteFont.GetAddressOf());
    m_device.spriteBatch->Begin(m_device.dx12->GetCommandList().Get());

    //  シーンごとに分岐
    switch (m_type)
    {
    case SceneType::Title:
        m_title->DrawString();
        break;
    case SceneType::GameScene:
        m_gameScene->DrawString();
        break;
    case SceneType::Result:
        m_result->DrawString();
        break;
    default:
        break;
    }

    //  文字を描画した後の処理
    m_device.spriteBatch->End();
}

//  シーンの変更
void SceneController::ChangeScene(const SceneType _type)
{
    //  現在のシーンを設定
    m_type = _type;

    //  シーンの初期化とBGMの再生を始める
    //  シーンごとに分岐
    switch (m_type)
    {
    case SceneType::Title:
        m_title->Reset();
        m_title->StartBGM();
        break;
    case SceneType::GameScene:
        m_gameScene->Reset();
        m_gameScene->StartBGM();
        break;
    case SceneType::Result:
        m_result->Reset();
        m_result->SetUp(m_gameScene->GetScoreGetter());
        m_result->StartBGM();
        break;
    default:
        break;
    }
}