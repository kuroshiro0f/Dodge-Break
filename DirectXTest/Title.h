#pragma once

//  タイトルシーンの描画や更新を行うクラス
class Title
{
public:
    Title();
    ~Title();

    //  リセット
    void Reset();

    //  更新
    void Update();

    //  描画
    void Draw();

    //  文字描画
    void DrawString();

    //  BGMの再生開始
    void StartBGM();

    //  次へ進む
    void ToNext();
private:
    //  タイトルシーンの状態
    enum class TitleState
    {
        Play,       //  タイトルシーン再生中
        PressButton,        //  ボタンが押された
        ReadyToChangeScene, //  シーン遷移の準備

        None
    };
    TitleState m_state;

    //  再生するBGMのハンドル
    int m_bgmHandle;

    //  クラスへのポインタ
    class Mouse* m_mouse;       //  マウスの座標
    class StringDrawer* m_string;     //  文字の描画
    class Timer* m_waitTimer;   //  待機時間計測
    class TitleUI* m_ui;        //  UI

    //  シングルトンクラスへの参照
    class SceneController& m_sceneController;   //  シーンの管理  
    class Sound& m_sound;       //  音の管理
};