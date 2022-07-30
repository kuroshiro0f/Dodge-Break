#pragma once
#include <unordered_map>

enum class ScoreType;
//  リザルトでのスコア表示
class ResultScoreUI
{
public:
    ResultScoreUI();
    ~ResultScoreUI();

    //  リセット
    void Reset();

    //  スコアの設定
    void SetUp(class IScoreGetter* _score);

    //  更新
    void Update();

    //  文字描画
    void DrawString();

    //  スコア表示は終了したか
    bool IsFinishDisplayScore() { return m_isFinishDisplayScore; }
private:
    //  スコアの表示アニメーション
    void PlayAnimationScore(const ScoreType _scoreType);

    //  スコア表示は終了したか
    bool m_isFinishDisplayScore;

    //  実際のスコア
    std::unordered_map<ScoreType, int> m_actualScore;
    //  表示するスコア
    std::unordered_map<ScoreType, int> m_displayScore;

    //  スコアの表示状態
    ScoreType m_displayScoreType;

    //  文字描画クラスのインスタンスを保持するポインタ
    class StringDrawer* m_string;

    //  サウンド再生のクラスのインスタンスをシングルトンで取得
    class Sound& m_sound;
};