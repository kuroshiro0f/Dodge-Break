#pragma once
#include <unordered_map>

enum class ScoreType;
//  スコア量を得るためのインターフェース
class IScoreGetter
{
public:
    IScoreGetter();
    virtual ~IScoreGetter() {};

    //  スコア量を得る関数
    virtual int GetScore(const ScoreType _type) = 0;
    //  スコアの配列を返す
    virtual std::unordered_map<ScoreType, int> GetScoreList() = 0;
};