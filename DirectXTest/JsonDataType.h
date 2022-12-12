#pragma once

//  jsonファイルでまとめたデータの種類
enum class JsonDataType
{
    None = 0,   //  初期化用

    Player,     //  プレイヤーのパラメータ
    Energy,         //  エネルギーのパラメータ
    Enemy,          //  エネミーのパラメータ
    EnemyDefeater,  //  塔のパラメータ

    //  攻撃
    StraightAttack, //  直線攻撃
    FannelAttack,   //  ファンネル攻撃
    ZigzagAttack,   //  ジグザグ攻撃
    SpreadAttack,   //  拡散攻撃
    LineAttack,     //  弾を並べて転がす攻撃
    BeamAttack,     //  ビーム攻撃

    Wave,           //  ウェーブのデータ
    Stage           //  ステージを分割したときの座標データ
};