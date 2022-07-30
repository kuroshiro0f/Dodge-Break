#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  リザルトUIのデータ
namespace ResultUIData
{
    //  背景関連
    //  座標
    const XMFLOAT2 BACK_SPRITE_POS = { 0,0 };
    //  拡大率
    const float BACK_SPRITE_MAGNIFICATION = 1.0f;

    //  RESULT文字
    const char* RESULT_STRING = "RESULT";
    //  RESULT文字の座標
    const XMFLOAT2 RESULT_STRING_POS = { 750,50 };
    //  RESULT文字の拡大率
    const float RESULT_STRING_RATE = 1.5f;

    //  文字表示からスコア表示に移るまでの時間
    const float TRANGITION_APPEAR_STRING_FOR_SCORE_TIME = 100.0f;
    //  スコア表示から評価の表示に移るまでの時間
    const float TRANGITION_SCORE_FOR_EVALUATE_TIME = 100.0f;

    //  文字の点滅速度
    const float FLASHING_SPEED_DEFAULT = 0.01f;
    //  文字の点滅速度の加速時の加速率
    const float UP_FLASHING_SPEED_MAGNIFICATION = 4.0f;
    //  文字の点滅のアルファ値の最大値と最小値
    const float ALFA_VALUE_MAX = 1.0f;
    const float ALFA_VALUE_MIN = 0.1f;
}