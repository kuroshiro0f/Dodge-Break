#pragma once
#include <DirectXMath.h>
#include <iostream>
#include <memory>

using namespace DirectX;

enum class GameSceneState;
//  ゲームシーンの描画や更新を行うクラス
class GameScene
{
public:
    GameScene();
    ~GameScene();

    //  初期化
    void Init();
    //  リセット
    void Reset();

    //  更新
    void Update(const float _deltaTime);

    //  描画
    void DrawModel();
    void DrawSprite();
    void DrawString();

    //  BGMの再生開始
    void StartBGM();

    //  タイトルへ移行
    //  NOTE: ゲームオーバー時の処理
    void ChangeTitle();
    //  ゲームシーンへ移行
    //  NOTE: ゲームオーバー時の処理
    void RetryGameScene();

#if _DEBUG
    //  ゲームの一時停止
    //  NOTE: 現在の仕様ではゲーム内のオブジェクトの動きを止めるだけで、
    //        ゲームそのものの経過時間を止めることはできないため、
    //        一時停止しても、時間が経過すれば新しいエネミーが登場します。
    //        そのため、デバッグ専用の機能としております。
    void Pause();

    //  一時停止中にRestartボタンを押したらゲームを再開
    void Restart();

    //  チェックポイントからゲームシーンを再開
    //  次のチェックポイントへ
    void RestartNextCheckPoint();
    //  前のチェックポイントへ
    void RestartPrevCheckPoint();

    //  jsonファイルの再読み込み
    void ReloadFile();
#endif

    //  スコアを得るためのクラスのインスタンスを返す
    class IScoreGetter* GetScoreGetter();
private:
    //  シーンの状態毎の更新
    void UpdateStartScene();
    void UpdatePlayScene();
    void UpdateGameOverScene();
    void UpdateGameClearScene();

    //  クリア時の処理
    void GameClear();

    //  ゲームオーバー時の処理
    void GameOver();

    //  ゲームオーバー時の処理の状態
    enum class GameOverState
    {
        DisplayScore,   //  スコア表示
        ChangeTitle,    //  タイトルへ移行
        RetryGameScene, //  ゲームシーンやり直し

        None
    };
    GameOverState m_gameOverState;

    //  再生するBGMのハンドル
    int m_bgmHandle;

    //  カメラの座標の移動量
    XMFLOAT3 m_cameraMovementAmount;

    //  デルタタイム
    float m_deltaTime;

    //  現在のシーンの状態
    GameSceneState m_state;

    //  クラスへのポインタ
    //  各オブジェクトの管理
    class PlayerManager* m_playerManager;
    class EnemyManager* m_enemyManager;
    class EnemyDefeaterManager* m_enemyDefeaterManager;

    //  文字描画
    class StringDrawer* m_string;
    //  地面
    class Ground* m_ground;
    //  出現させるオブジェクトの管理
    class AppearEnemyDirector* m_appearDirector;
    //  UI
    class GameSceneUI* m_ui;
    //  シーンの各演出の時間計測
    class Timer* m_directSceneTimer;
    //  ゲームの経過時間計測
    Timer* m_gameTimer;
    //  カメラの移動
    class Camera* m_camera;
    //  エネミーの攻撃の管理
    class EnemyAttackDirector* m_enemyAttack;
    //  ウェーブ変更の指示を行うクラス
    class WaveDirector* m_waveDirector;
    //  スコアのクラス
    class Score* m_score;
    //  失敗時のリザルトのUIクラス
    class FailedResultUI* m_failedResultUI;
    //  イベント通知
    std::shared_ptr<class EventNotificator> m_eventNotificator;
    //  塔の座標を知らせるクラス
    std::shared_ptr<class EnemyDefeaterMapper> m_enemyDefeaterMapper;

    //  シングルトンクラスへの参照
    class Device& m_device;         //  デバイスの管理
    class SceneController& m_sceneController;   //  シーンの管理を行う
    class Sound& m_sound;       //  音の管理を行う
    class CollisionManager& m_collision;    //  衝突判定を行う
    class UserInputHandler& m_userInputHandler; //  入力検知
    class GameSpeedOperator& m_speedOperator;
};