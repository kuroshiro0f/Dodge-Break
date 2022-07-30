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
private:
    //  クリックされたか
    bool m_isClick;

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
    class KeyBoard& m_keyboard; //  キーの入力検知
};