#pragma once

//  エネミーの撃破数を得るためのインターフェース
class IDefeatEnemyNumGetter
{
public:
    IDefeatEnemyNumGetter();
    virtual ~IDefeatEnemyNumGetter() {};

    //  エネミーの撃破数を取得
    virtual int GetDefeatEnemyNum()const = 0;
};