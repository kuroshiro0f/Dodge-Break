#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  エネルギーゲージUIのデータ
namespace EnergyGaugeUIData
{
    //  エネルギー画像の座標
    const XMFLOAT2 ENERGY_FRAME_POS = { 630.0f,35.0f };
    const XMFLOAT2 ENERGY_POS = { ENERGY_FRAME_POS.x + 5,ENERGY_FRAME_POS.y + 6 };
    //  エネルギーの画像の拡大率
    const float ENERGY_MAGNIFICATION = 1.0f;
    //  エネルギーゲージの画像の基本の伸縮率
    const float ENERGY_GAUGE_UI_RATE = 1.0f;

    //  エネルギー量を割合に変更するための掛ける数
    const float ENERGY_AMOUNT_TO_PERCENTAGE = 0.01f;

    //  アニメーションにおけるエネルギーの1フレームでの増加量
    const float ENERGY_ADD_SPEED = 0.5f;
    //  アニメーション時に明滅させるために、画像の発行時の色として設定する値
    const DirectX::XMVECTOR FLASH_COLOR = { 2.0f,2.0f,2.0f,1.0f };
}