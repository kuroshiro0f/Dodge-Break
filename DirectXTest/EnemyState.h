#pragma once

//  エネミーの状態
enum class EnemyState
{
    Appear,     //  出現中
    Attack,     //  攻撃中
    Exit,       //  退場中
    StandBy     //  待機中

    , None
};