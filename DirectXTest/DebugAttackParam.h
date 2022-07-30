#pragma once
#if _DEBUG

//  攻撃の実験用パラメータ
namespace DebugAttackParam
{
    const int NUM = 3;      //  攻撃に使う弾の数
    const float ALIVE_TIME = 300.0f;     //  生存時間
    const float SHOOT_INTERVAL = 400.0f;    //  発射間隔
    const float SHOOT_ANGLE = PI * 0.25;    //  隣の弾との発射角度の差
    const float SPEED = 0.5f;        //  移動速度
    const float RADIUS = 10.0f;       //  衝突判定の大きさの半径
}
#endif