#pragma once

//  プレイヤーのデータ
namespace PlayerData
{
    //  ステージのサイズ
    static const float MAP_SIZE = 144.0f;

    //  無敵時の点滅間隔
    static const float INVINCIBLE_FLASHING_INTERVAL = 8.0f;

    //  地面からの距離
    static const float DISTANCE_FROM_GROUND = 0.75f;

    //  影のY座標
    static const float SHADOW_POS_Y = 0.1f;

    //  エネルギーのエフェクトのロード数
    static const int ENERGY_EFFECT_NUM = 10;

    //  エフェクトの位置調整用
    //  NOTE: プレイヤーの登場エフェクト時に、影がエフェクトからはみ出るため、エフェクトの位置を調整
    static const float FIX_EFFECT_POS_Z = -1.8f;

    //  出現時間
    static const float APPEAR_TIMER = 40.0f;

    //  退場時間
    static const float EXIT_TIMER = 30.0f;

    //  再開時間
    static const float RESTART_TIME = 100.0f;

    //  半分にする
    static const float HALF_SCALE = 0.5f;
}