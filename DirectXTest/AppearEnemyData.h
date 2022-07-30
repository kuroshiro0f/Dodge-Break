#pragma once
#include <deque>
#include <DirectXMath.h>

#include "EnemyType.h"
#include "EnemyAttackType.h"
#include "CollisionType.h"

using namespace DirectX;

//  出現させるエネミーのデータ
struct AppearEnemyData
{
    //  エネミーの種類
    EnemyType enemyType;
    //  エネミーの攻撃の種類
    EnemyAttackType enemyAttackType;
    //  エネミーの攻撃のパターンID
    int attackPetternID;
    //  攻撃回数
    //  NOTE: 攻撃回数が0以下なら、ウェーブが終わるまで退場はしない。
    int attackNum;
    //  退場までの時間
    float exitTime;
    //  バースト撃ちの時間
    float burstTime;
    //  バースト撃ちの回数
    //  NOTE: バースト撃ちの回数が0以下ならバースト撃ちの処理は行わない。
    int burstNum;

    //  移動先
    std::vector<int> movePoint;
    //  ベジェ曲線での移動用制御点
    //  NOTE: 直線移動の場合は-1を設定する
    std::vector<int> controlPoint;
    //  1ループにおける移動割合
    //  NOTE: 総移動距離を1.0として、そのうちのmoveRate分移動する
    //  NOTE: 次の座標に瞬時に移動するときは、moveRateを1.0fに設定する
    std::vector<float> moveRate;
    //  移動回数
    int moveNum;

    //  登場時の移動先
    std::vector<int> appearMovePoint;
    //  登場時のベジェ曲線での移動用制御点
    std::vector<int> appearControlPoint;
    //  登場時の1ループにおける移動割合
    //  NOTE: 総移動距離を1.0として、そのうちのmoveRate分移動する
    //  NOTE: 次の座標に瞬時に移動するときは、moveRateを1.0fに設定する
    std::vector<float> appearMoveRate;
    //  登場時の移動回数
    int appearMoveNum;

    //  発射間隔
    float shootInterval;
    //  出現ウェーブ番号
    int waveNum;
    //  出現時間
    float appearTime;
};