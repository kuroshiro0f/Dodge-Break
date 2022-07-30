#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  マップチップのデータ
struct MapData {
    const int MAP_TOTAL_NUM = 7;                 //  マップの総数
    const int RANDOM_MAP_NUM = 5;                //  ランダムに生成されるマップの種類の総数
    const XMFLOAT3 TILE_SIZE = { 16,0,16 };      //  タイルのサイズ
    const int PLAYER = 1;                        //  マップチップの番号
    const int ENEMY_RED = 2;
    const int ENEMY_BLUE = 3;
    const int ENEMY_YELLOW = 4;
    const int OBS_RED = 8;
    const int OBS_BLUE = 9;
    const int OBS_YELLOW = 10;
    const int OBS_THREE_COLORS = 12;
};