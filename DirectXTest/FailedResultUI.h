#pragma once

enum class ScoreType;
//  失敗時のリザルトのUI
//  NOTE: ゲームシーンを止めて表示する
class FailedResultUI
{
public:
    FailedResultUI();
    ~FailedResultUI();

    //  リセット
    void Reset();

    //  UIの設定
    void SetUp(class IScoreGetter* _score);

    //  更新
    void Update();

    //  描画
    void DrawSprite();  //  画像描画
    void DrawString();  //  文字描画
private:
    //  フェードアウト
    void FadeOut();

    //  フェードアウト用アルファ値
    float m_alfa;

    //  色の設定を行うクラスのインスタンスを保持するポインタ
    class ColorValue* m_color;
    //  スコアのUIを表示するクラスのインスタンスを保持するポインタ
    class ResultScoreUI* m_scoreUI;
    //  TipsのUIを表示するクラスのインスタンスを保持するポインタ
    class TipsUI* m_tipsUI;
    //  状態の遷移を管理する時間計測クラスのインスタンスを保持するポインタ
    class Timer* m_displayStateTimer;
    //  文字描画のクラスのインスタンスを保持するポインタ
    class StringDrawer* m_string;

    //  画像の描画のクラスのインスタンスをシングルトンで取得
    class SpriteDrawer& m_sprite;

    //  表示の状態
    enum class DisplayState
    {
        FadeOut,    //  フェードアウト
        AppearString,   //  文字の表示
        Score,          //  スコアの値の変化
        Tips            //  Tipsの表示

        , None
    };
    DisplayState m_displayState;
};