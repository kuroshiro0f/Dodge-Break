#pragma once

#include <DirectXMath.h>

using namespace DirectX;

//  NOTE: ここは全体的に作り変える予定なので命名修正はスキップ
//  TODO: 全体的な修正

//  ライフの情報
namespace LifeUIData
{
    const XMFLOAT2 POS = { 15.0f,1000.0f };        //  座標
    const float MAGNIFICATION = 0.05f;            //  拡大率

    const XMFLOAT2 RESULT_POS = { 580.0f,400.0f };
    const float RESULT_MAGNIFICATION = 0.04f;
    const float SPACE = 50.0f;

    const float LIFE_SCORE_ADD_POINT = 100000;
};