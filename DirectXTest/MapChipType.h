#pragma once

//  読み込むマップチップの種類
enum class MapChipType {
    Obstacle = 0,    //  障害物のマップ

    First,           //  最初のファイル

    Random1,         //  ランダムに表示するファイル
    Random2,
    Random3,
    Random4,
    Random5,

    Last             //  最後のファイル
};