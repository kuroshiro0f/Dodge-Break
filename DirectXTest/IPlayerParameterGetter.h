#pragma once

//  プレイヤーのパラメータを取得する関数をまとめたインターフェース
class IPlayerParameterGetter
{
public:
    IPlayerParameterGetter();
    virtual ~IPlayerParameterGetter() {};

    //  残機量を得る関数
    virtual int GetLife()const = 0;

    //  エネルギー量を得る関数
    virtual int GetEnergy()const = 0;

    //  回避回数を取得
    virtual int GetDodgeNum()const = 0;
};