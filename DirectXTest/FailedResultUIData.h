#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  失敗時のリザルトUIのデータ
namespace FailedResultUIData
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
    //  スコア表示からTipsの表示に移るまでの時間
    const float TRANGITION_SCORE_FOR_TIPS_TIME = 100.0f;

    //  アルファ値関連
    //  フェードアウトで使用
    //  最大値
    const float ALFA_MAX = 0.6f;
    //  最小値
    const float ALFA_MIN = 0;
    //  アルファ値の上昇速度
    const float INCREASE_ALFA_SPEED = 0.01f;
}