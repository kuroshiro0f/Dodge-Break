#pragma once

class GameSceneScoreUI
{
public:
    GameSceneScoreUI();
    ~GameSceneScoreUI();

    //  初期化
    void Init(class IScoreGetter* _scoreGetter);
    //  リセット
    void Reset();

    //  更新
    void Update();

    //  文字描画
    void DrawString();
private:
    //  スコアの増加アニメーション
    void PlayAnimationScoreIncrease();

    //  表示するスコア
    //  NOTE: 表示する値を一気に増加させるのではなく、変動がわかるように
    int m_displayScore;
    //  実際のスコア
    int m_actualScore;
    //  アニメーション開始時のスコア
    int m_startAnimationScore;

    //  総スコア
    int m_totalScore;

    //  スコア量を取得するためのクラス
    class IScoreGetter* m_scoreGetter;
    //  文字描画のためのクラスのインスタンスを保持するポインタ
    class StringDrawer* m_string;
};