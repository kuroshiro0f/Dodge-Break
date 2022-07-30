#include "GameScene.h"

#include "CollisionManager.h"
#include "EnemyDefeaterManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "EnemyType.h"
#include "DodgeRange.h"
#include "EnemyAttackDirector.h"
#include "Energy.h"
#include "SoundType.h"
#include "GameSceneState.h"
#include "AppearEnemyData.h"
#include "GameSceneParam.h"
#include "SceneParam.h"
#include "SceneType.h"
#include "AppearEnemyDirector.h"
#include "WaveDirector.h"
#include "Dx12Wrapper.h"
#include "Camera.h"
#include "Ground.h"
#include "StringDrawer.h"
#include "Timer.h"
#include "GameSceneUI.h"
#include "EventNotificator.h"
#include "EventType.h"
#include "EnemyDefeaterMapper.h"
#include "Score.h"
#include "FailedResultUI.h"

#include "IWaveConditionGetter.h"
#include "IScoreGetter.h"
#include "IDefeatEnemyNumGetter.h"
#include "IPlayerParameterGetter.h"

#include "Singleton.h"
#include "Device.h"
#include "SceneController.h"
#include "Sound.h"
#include "GameSpeedOperator.h"
#include "KeyBoard.h"

#if _DEBUG
#include "DataReLoader.h"
#include "LoadJson.h"
#endif

GameScene::GameScene()
    :m_state(GameSceneState::Start)
    , m_deltaTime(0)
    , m_device(Singleton<Device>::GetInstance())
    , m_collision(Singleton<CollisionManager>::GetInstance())
    , m_sound(Singleton<Sound>::GetInstance())
    , m_sceneController(Singleton<SceneController>::GetInstance())
    , m_speedOperator(Singleton<GameSpeedOperator>::GetInstance())
    , m_keyboard(Singleton<KeyBoard>::GetInstance())
    , m_camera(new Camera())
    , m_enemyManager(new EnemyManager())
    , m_enemyDefeaterManager(new EnemyDefeaterManager())
    , m_appearDirector(new AppearEnemyDirector())
    , m_playerManager(new PlayerManager())
    , m_waveDirector(new WaveDirector())
    , m_score(new Score())
    , m_string(new StringDrawer())
    , m_ground(new Ground())
    , m_directSceneTimer(new Timer())
    , m_gameTimer(new Timer())
    , m_ui(new GameSceneUI())
    , m_enemyAttack(new EnemyAttackDirector())
    , m_failedResultUI(new FailedResultUI())
    , m_bgmHandle(0)
    , m_cameraMovementAmount({ 0,0,0 })
{
    //  処理なし
}

GameScene::~GameScene()
{
    //  メモリの解放
    delete m_playerManager;
    delete m_enemyManager;
    delete m_enemyDefeaterManager;
    delete m_string;
    delete m_ground;
    delete m_appearDirector;
    delete m_directSceneTimer;
    delete m_gameTimer;
    delete m_ui;
    delete m_camera;
    delete m_enemyAttack;
    delete m_failedResultUI;
}

//  初期化
void GameScene::Init()
{
    //  イベントの登録と通知
    m_eventNotificator = std::make_shared<EventNotificator>(EventNotificator());
    //  EnemyDefeaterの座標を知らせるクラスのインスタンスを生成
    m_enemyDefeaterMapper = std::make_shared<EnemyDefeaterMapper>(EnemyDefeaterMapper());

    //  ゲームオーバーの処理をする関数のポインタを生成
    std::function<void()> gameOver = std::bind(&GameScene::GameOver, this);
    //  プレイヤーマネージャー初期化
    m_playerManager->Init(gameOver, m_eventNotificator, m_enemyDefeaterMapper);

    //  エネミーマネージャー初期化
    m_enemyManager->Init(m_eventNotificator);

    //  エネミーを倒す関数のポインタを生成
    std::function <void(EnemyType)> defeatFunc = std::bind(&EnemyManager::DefeatEnemy, m_enemyManager, std::placeholders::_1);
    //  エネミーが出現しているか確認するための関数のポインタを生成
    std::function<bool(const EnemyType)> isDisplayEnemyType = std::bind(&EnemyManager::IsDisplayEnemyType, m_enemyManager, std::placeholders::_1);
    m_enemyDefeaterManager->Init(m_eventNotificator, m_enemyDefeaterMapper, defeatFunc, isDisplayEnemyType);

    //  エネミーを出現させる関数のポインタを生成
    std::function <void(const std::vector<AppearEnemyData*>&)> appearEnemy = std::bind(&EnemyManager::AppearEnemy, m_enemyManager, std::placeholders::_1);
    //  準備中の塔を起動させる関数のポインタを生成
    std::function <void()> activateStandByEnemyDefeater = std::bind(&EnemyDefeaterManager::ActivateStandByEnemyDefeater, m_enemyDefeaterManager);
    //  AppearObjectDirectorクラスの初期化
    m_appearDirector->Init(appearEnemy, activateStandByEnemyDefeater, m_eventNotificator);

    //  ゲームクリアの処理をする関数のポインタを生成
    std::function<void()> clear = std::bind(&GameScene::GameClear, this);
    //  エネミーの登場を開始する関数のポインタを生成
    std::function <void(int)> startAppearEnemyFunc = std::bind(&AppearEnemyDirector::Start, m_appearDirector, std::placeholders::_1);
    //  エネミーが画面上に存在するか確認する関数のポインタを生成
    std::function <bool()> isDisplayEnemy = std::bind(&EnemyManager::IsDisplayEnemy, m_enemyManager);
    //  エネミーを退場させる関数のポインタを生成
    std::function <void()> exitEnemy = std::bind(&EnemyManager::ExitAllEnemy, m_enemyManager);
    //  ウェーブの初期化
    m_waveDirector->Init(startAppearEnemyFunc, isDisplayEnemy, exitEnemy, clear, m_eventNotificator);

    //  スコアの初期化
    m_score->Init(m_playerManager->GetPlayerParameterGetter(), static_cast<IDefeatEnemyNumGetter*>(m_enemyManager), m_eventNotificator);

    //  地面クラスの初期化
    m_ground->Init();

    //  UIクラスの初期化
    m_ui->Init(m_playerManager->GetPlayerParameterGetter(), static_cast<IScoreGetter*>(m_score), m_eventNotificator);
}

//  リセット
void GameScene::Reset()
{
    //  各オブジェクトのマネージャーリセット
    m_playerManager->Reset();
    m_enemyManager->Reset();
    m_enemyDefeaterManager->Reset();

    //  AppearObjectDirectorクラスのリセット
    m_appearDirector->Reset();

    //  ウェーブのリセット
    m_waveDirector->Reset();
    m_appearDirector->Reset();

    //  スコアのリセット
    m_score->Reset();

    //  UIクラスのリセット
    m_ui->Reset();
    m_failedResultUI->Reset();

    //  時間計測クラスのリセット
    m_directSceneTimer->Reset();
    m_gameTimer->Reset();

    //  ゲームシーンの状態をStartに変更
    m_state = GameSceneState::Start;

    //  カメラの初期位置をセット
    m_camera->SetUpCamera(m_playerManager->GetPlayerPos());

    //  時間計測の開始
    m_directSceneTimer->Start();
}

//  更新
void GameScene::Update(const float _deltaTime)
{
#if _DEBUG
    //  デバッグ用の処理を行う
    DebugUpdate();
#endif

    //  DeltaTimeの更新
    m_deltaTime = _deltaTime;

    //  ゲームシーンの状態毎に処理を分岐
    switch (m_state)
    {
    case GameSceneState::Start:
        UpdateStartScene();
        break;
    case GameSceneState::PlayGame:
        UpdatePlayScene();
        break;
    case GameSceneState::GameOver:
        UpdateGameOverScene();
        break;
    case GameSceneState::GameClear:
        UpdateGameClearScene();
        break;
    default:
        break;
    }
}

//  シーン開始時の更新
void GameScene::UpdateStartScene()
{
    //  プレイヤーの登場開始時間になったら
    if (m_directSceneTimer->GetElapseTime() >= GameSceneParam::PLAYER_APPEARANCE_TIME)
    {
        //  プレイヤーの登場
        m_playerManager->AppearPlayer();
        //  カメラの移動開始時間になったら  
        if (m_directSceneTimer->GetElapseTime() >= GameSceneParam::PLAYER_APPEARANCE_TIME + GameSceneParam::START_CAMERA_MOVE_TIME)
        {
            //  カメラの移動開始
            m_camera->PlayAnimation(m_deltaTime);
            //  カメラの移動が終わったら
            if (m_camera->IsFinishAnimation())
            {
                //  ウェーブ開始
                m_waveDirector->Start();
                //  経過時間計測開始
                m_gameTimer->Start();
                //  ゲームシーンの状態をPlayGameへ変更
                m_state = GameSceneState::PlayGame;
            }
        }
    }
}

//  プレイシーン中の更新
void GameScene::UpdatePlayScene()
{
    //  出現させるオブジェクトの更新
    m_waveDirector->Update();
    m_appearDirector->Update();
    //  プレイヤーの更新
    m_playerManager->Update(m_deltaTime);
    //  エネミーの更新
    m_enemyManager->Update(m_playerManager->GetPlayerPos(), m_deltaTime);
    //  塔の更新
    m_enemyDefeaterManager->Update(m_deltaTime);
    //  当たり判定を更新
    m_collision.Update();
    //  UIを更新
    m_ui->Update();
}

//  ゲームオーバー時の更新
void GameScene::UpdateGameOverScene()
{
    if (m_directSceneTimer->GetElapseTime() < GameSceneParam::FAILED_RESULT_TRANSITION_TIME)
    {
        //  プレイヤーの更新
        m_playerManager->Update(m_deltaTime);
        //  エネミーの更新
        m_enemyManager->Update(m_playerManager->GetPlayerPos(), m_deltaTime);
        //  塔の更新
        m_enemyDefeaterManager->Update(m_deltaTime);
    }
    else
    {
        //  プレイヤーの更新
        m_playerManager->Update(GameSceneParam::FAILED_RESULT_GAME_TIME_SPEED);
        //  エネミーの更新
        m_enemyManager->Update(m_playerManager->GetPlayerPos(), GameSceneParam::FAILED_RESULT_GAME_TIME_SPEED);
        //  塔の更新
        m_enemyDefeaterManager->Update(GameSceneParam::FAILED_RESULT_GAME_TIME_SPEED);
        //  失敗時のリザルトUIを更新
        m_failedResultUI->Update();
        //  キーが押されたらシーン移行
        if (m_keyboard.IsPressKey(m_keyboard.KeyBind::ChangeTitle))
        {
            m_sound.Stop(SoundType::GameSceneBGM, m_bgmHandle);
            //  タイトルへ移行
            m_sceneController.ChangeScene(SceneType::Title);
        }
        else if (m_keyboard.IsPressKey(m_keyboard.KeyBind::ChangeGameScene))
        {
            m_sound.Stop(SoundType::GameSceneBGM, m_bgmHandle);
            //  ゲームシーンへ移行
            m_sceneController.ChangeScene(SceneType::GameScene);
        }
    }
}

//  ゲームクリア時の更新
void GameScene::UpdateGameClearScene()
{
    //  プレイヤーの退場
    m_playerManager->ExitPlayer(m_deltaTime);
    //  UIを更新
    m_ui->Update();
    //  一定時間後にフェードアウト開始
    if (m_directSceneTimer->GetElapseTime() >= GameSceneParam::TRANGITION_TIME_CLEAR_FOR_FADE_OUT)
    {
        m_ui->FadeOut();
        //  フェードアウト終了後、BGMを止めてリザルトへ移行
        if (m_ui->IsFinishFadeOut())
        {
            m_sound.Stop(SoundType::GameSceneBGM, m_bgmHandle);
            m_sceneController.ChangeScene(SceneType::Result);
        }
    }
}

//  モデルの描画
void GameScene::DrawModel()
{
    //  各モデルの描画
    m_playerManager->DrawPlayer();
    m_enemyManager->DrawEnemy();
    m_enemyDefeaterManager->DrawEnemyDefeater();
    m_ground->DrawGround();
}

//  画像の描画
void GameScene::DrawSprite()
{
    if (m_state != GameSceneState::GameOver)
    {
        //  UIの描画
        m_ui->DrawSprite();
    }
    else
    {
        //  失敗時のリザルトUIの描画
        m_failedResultUI->DrawSprite();
    }
}

//  文字の描画
void GameScene::DrawString()
{
    //  ゲームシーンの状態毎に処理を分岐
    switch (m_state)
    {
    case GameSceneState::Start:
        m_ui->DrawString();
        break;
    case GameSceneState::PlayGame:
        m_ui->DrawString();
        break;
    case GameSceneState::GameOver:
        //  失敗時のリザルトUIの描画
        m_failedResultUI->DrawString();
        break;
    default:
        break;
    }

  //  デバッグ時に画面上に表示するデータ
#if _DEBUG
  //m_string->DrawStringBlackAndYellowForFewNumber(m_playerManager->GetPlayerPos().x, { 0,0 });
  //m_string->DrawStringBlackAndYellowForFewNumber(m_playerManager->GetPlayerPos().z, { 0,100 });

    ////  デルタタイムの表示
    //m_string->DrawStringBlackAndYellowForFewNumber(m_deltaTime, { 0,200 });

    ////  回避数の表示
    //m_string->DrawStringBlackAndYellowForNumber(m_dodgeRange->count, { 0,0 });

    //  リスタート時のチェックポイントを指定
   /* m_string->DrawStringBlackAndYellow("CheckPoint:", { 1520.0f,0 });
    m_string->DrawStringBlackAndYellowForNumber(m_appearDirector->GetDebugCheckPointNum() + 1, { 1800.0f,0.0f });*/
#endif
}

//  BGMの再生開始
void GameScene::StartBGM()
{
    //  BGMの再生
    m_bgmHandle = m_sound.Play(SoundType::GameSceneBGM, true, true);
}

//  スコアを得るためのクラスのインスタンスを返す
IScoreGetter* GameScene::GetScoreGetter()
{
    return static_cast<IScoreGetter*>(m_score);
}

//  ゲームクリア時の処理
void GameScene::GameClear()
{
    //  ゲームシーンの状態をGameClearへ変更
    m_state = GameSceneState::GameClear;

    //  スコアの最終更新
    m_score->CalculateResultScore(m_waveDirector->GetAliveTime(), m_playerManager->GetPlayerParameterGetter()->GetLife());

    //  時間計測の開始
    m_directSceneTimer->Start();
}

//  ゲームオーバー時の処理
void GameScene::GameOver()
{
    //  ゲームシーンの状態をGameOverへ変更
    m_state = GameSceneState::GameOver;

    //  スコアの最終更新
    m_score->CalculateResultScore(m_waveDirector->GetAliveTime(), m_playerManager->GetPlayerParameterGetter()->GetLife());

    //  失敗時のリザルトUIの設定
    m_failedResultUI->SetUp(static_cast<IScoreGetter*>(m_score));

    //  時間計測の開始
    m_directSceneTimer->Start();
}

//  チェックポイントからゲームシーンを再開
#if _DEBUG
void GameScene::DebugUpdate()
{
    //  ゲームの一時停止
    //  NOTE: 現在の仕様ではゲーム内のオブジェクトの動きを止めるだけで、
    //        ゲームそのものの経過時間を止めることはできないため、
    //        一時停止しても、時間が経過すれば新しいエネミーが登場します。
    //        そのため、デバッグ専用の機能としております。
    //  Pauseボタンを押したらゲームを一時停止
    if (m_keyboard.IsPressKey(m_keyboard.KeyBind::Pause) && !m_speedOperator.IsPause())
    {
        m_speedOperator.Pause();
    }
    //  一時停止中にRestartボタンを押したらゲームを再開
    else if (m_keyboard.IsPressKey(m_keyboard.KeyBind::Restart) && m_speedOperator.IsPause())
    {
        m_speedOperator.ReStart();
    }

    //  チェックポイントからゲームシーンを再開
    //  次のチェックポイント
    if (m_keyboard.IsPressKey(m_keyboard.KeyBind::RestartNextPoint))
    {
        RestartNextCheckPoint();
    }
    //  前のチェックポイント
    if (m_keyboard.IsPressKey(m_keyboard.KeyBind::RestartPreviousPoint))
    {
        RestartPrevCheckPoint();
    }

    //  外部ファイルの再読み込みを行う
    if (m_keyboard.IsPressKey(m_keyboard.KeyBind::ReLoadFile))
    {
        //  jsonファイルの再読み込み
        LoadJson& loadJson = Singleton<LoadJson>::GetInstance();
        loadJson.Load();
        //  ファイルから読み込んだデータをロード
        DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
        dataReLoader.ReLoad();
    }
}
//  次のチェックポイントへ
void GameScene::RestartNextCheckPoint()
{
    //  プレイヤーのリスタート処理
    m_playerManager->Restart();
    //  エネミーのリセット
    m_enemyManager->Reset();
    m_state = GameSceneState::PlayGame;
    m_appearDirector->RestartCheckPoint(GameSceneParam::NEXT_CHECK_POINT);
}
//  前のチェックポイントへ
void GameScene::RestartPrevCheckPoint()
{
    //  プレイヤーのリスタート処理
    m_playerManager->Restart();
    //  エネミーのリセット
    m_enemyManager->Reset();
    m_state = GameSceneState::PlayGame;
    m_appearDirector->RestartCheckPoint(GameSceneParam::PREV_CHECK_POINT);
}
#endif