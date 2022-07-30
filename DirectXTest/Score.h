#pragma once
#include <memory>
#include <unordered_map>

#include "IScoreGetter.h"

enum class ScoreType;
//  スコアを計算し、保持するクラス
class Score :public IScoreGetter
{
public:
    Score();
    ~Score();

    //  初期化
    void Init(class IPlayerParameterGetter* _playerParameterGetter, class IDefeatEnemyNumGetter* _defeatEnemyNumGetter, const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  回避スコアの更新
    void UpdateDodgeScore();
    //  エネミーの撃破数スコアの更新
    void UpdateDefeatEnemyScore();

    //  リザルトでの最終スコア算出
    void CalculateResultScore(const int _aliveTime, const int _lifeNum);

    //  スコア量を返す
    int GetScore(const ScoreType _type)override;
    //  スコアの配列を返す
    std::unordered_map<ScoreType, int> GetScoreList()override;
private:
    //  各スコアの算出
    void CalculateScore(const ScoreType _type, const int _num);

    //  スコアの表示に必要な値を格納する配列
    std::unordered_map<ScoreType, int> m_scoreData;
    //  スコアの計算時の倍率を格納する配列
    std::unordered_map<ScoreType, int> m_scoreMagnification;

    //  回避回数を得るためのクラスへのポインタ
    class IPlayerParameterGetter* m_playerParameterGetter;
    //  エネミーの撃破数を得るためのクラスへのポインタ
    IDefeatEnemyNumGetter* m_defeatEnemyNumGetter;
};