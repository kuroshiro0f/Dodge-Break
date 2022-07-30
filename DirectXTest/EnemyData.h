#pragma once

//  エネミーのデータ
namespace EnemyData
{
    //  移動速度
    const float SPEED = 0.03f;
    //  衝突時に戻る距離
    const float RETURN_MOVE_DISTANCE = 0.5f;
    //  半径
    const float RADIUS = 4.5f;
    //  数
    const int TOTAL_NUM = 100;

    //  地面からの距離
    const float DISTANCE_FROM_GROUND = 2.5f;
    //  影のY座標
    const float SHADOW_POS_Y = 0.1f;

    //  退場時の速度
    const float EXIT_SPEED = 0.01f;
    //  退場に必要な中央から画面外までの距離
    const float EXIT_DISTANCE_FROM_CENTER = 130.0f;

    //  最初に発射し始めるまでの追加時間
    const float FIRST_SHOOT_TIME_ADD = 50.0f;
    //  再開時に発射し始めるまでの追加時間
    const float RESTART_SHOOT_TIME_ADD = 50.0f;

    //  角度計算の基準となる角度
    //  NOTE: オブジェクトがZ平面の正の方向を向いている角度を基準としているので、その方向の単位ベクトルとして{0,0,1.0f}を使用
    const XMFLOAT3 CRITERION_ANGLE = { 0,0,1.0f };

    //  ステージ中心に現れたエネミーの退場方向への単位ベクトル
    const XMFLOAT3 EXIT_VEC_FOR_CENTER_ENEMY = { 0,0,1.0f };
};