#pragma once

//  リザルトシーンの描画や更新を行うクラス
class Result {
public:
    Result();
    ~Result();

    //  リセット
    void Reset();
    //  設定
    void SetUp(class IScoreGetter* _score);

    //  更新
    void Update();

    //  描画
    void DrawSprite();
    void DrawString();

    //  BGMの再生開始
    void StartBGM();

    //  次へ進む
    void ToNext();
private:
    //  リザルトシーンの状態
    enum class ResultState
    {
        Play,       //  リザルトシーン再生中
        PressButton,        //  ボタンが押された
        ReadyToChangeScene, //  シーン遷移の準備

        None
    };
    ResultState m_state;

    //  再生するBGMのハンドル
    int m_bgmHandle;

    //  クラスへのポインタ
    class ResultUI* m_ui;   //  UI
    class Mouse* m_mouse;   //  マウスの座標
    class StringDrawer* m_string; //  文字の描画
    class Timer* m_waitTimer;   //  待機時間計測

    //  シングルトンクラスへの参照
    class SceneController& m_sceneController;   //  シーンの管理
    class Sound& m_sound;       //  音の管理
};