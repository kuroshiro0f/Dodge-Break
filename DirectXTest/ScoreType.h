#pragma once

//  スコアの種類
enum class ScoreType
{
    AliveTime,  //  生存時間
    DodgeNum,       //  回避回数
    LifeNum,        //  残機数
    DefeatEnemyNum, //  エネミーの撃破数
    Total           //  合計

    , None
};