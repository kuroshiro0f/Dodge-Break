#pragma once

enum class ScoreType;
//  リザルトシーンのUIクラス
class ResultUI
{
public:
    ResultUI();
    ~ResultUI();

    //  リセット
    void Reset();

    //  UIの設定
    void SetUp(class IScoreGetter* _score);

    //  更新
    void Update(const bool _isClick);

    //  描画
    void DrawSprite();  //  画像描画
    void DrawString();  //  文字描画
private:
    //  点滅アニメーション
    void PlayAnimationFlashing(const float _flashingSpeed);
    //  透明化させるか
    bool m_isMakeTransparent;
    //  アルファ値
    float m_alfaValue;

    //  色の設定を行うクラスのインスタンスを保持するポインタ
    class ColorValue* m_color;
    //  スコアのUIを表示するクラスのインスタンスを保持するポインタ
    class ResultScoreUI* m_scoreUI;
    //  評価のUIを表示するクラスのインスタンスを保持するポインタ
    class ScoreEvaluationUI* m_ScoreEvaluationUI;
    //  状態の遷移を管理する時間計測クラスのインスタンスを保持するポインタ
    class Timer* m_displayStateTimer;
    //  文字描画のクラスのインスタンスを保持するポインタ
    class StringDrawer* m_string;

    //  画像の描画のクラスのインスタンスをシングルトンで取得
    class SpriteDrawer& m_sprite;

    //  表示の状態
    enum class DisplayState
    {
        AppearScoreString,      //  スコアの文字表示
        Score,      //  スコアの値変化
        Evaluate,   //  評価表示
        AppearGuideString       //  次のシーンに行くためのガイド表示

        , None
    };
    DisplayState m_displayState;
};