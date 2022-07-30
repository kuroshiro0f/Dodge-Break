#pragma once

//  ウェーブの状態を得るためのインターフェース
class IWaveConditionGetter
{
public:
    IWaveConditionGetter();
    virtual ~IWaveConditionGetter() {};

    //  時間を得る関数
    virtual float GetTime()const = 0;

    //  インターバル中か得る関数
    virtual bool IsInterval()const = 0;
};