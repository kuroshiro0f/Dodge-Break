#pragma once

//  エネミーの攻撃の種類
enum class EnemyAttackType
{
    None = 0,   //  初期化用

    //  プレイヤーの位置を参照する攻撃
    Straight,   //  直線攻撃
    Fannel,         //  上空に発射されて一度停止し、その後にプレイヤーを追尾する攻撃(ファンネル攻撃)
    Spread,         //  一定距離進んだ後、拡散する攻撃

    //  プレイヤーの位置にかかわらず、固定の動きをする攻撃
    Zigzag,         //  ジグザグ攻撃
    Line,           //  弾を並べて転がす攻撃

    Beam,       //  ビーム攻撃

    NoAttack        //  攻撃しない

#if _DEBUG
    ,Debug           //  実験用
#endif
};