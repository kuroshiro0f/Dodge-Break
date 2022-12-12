#pragma once

//  衝突判定のあるオブジェクトの名前
enum class ObjectTagName
{
    Player,         //  プレイヤー
    Enemy,          //  エネミー
    EnemyDefeater,  //  エネミーを倒すオブジェクト
    EnemyDefeaterActivateEffect, //  エネルギーが溜まった際に塔に飛んでいくエフェクト
    EnemyAttack,    //  エネミーの攻撃
    BeamAttack,     //  ビーム攻撃(NOTE: エネミーの攻撃ですが、他の攻撃とは様々な処理が変わるため、名前を分けております)
    DodgeRange,     //  回避範囲そのものの実体

    None            //  初期化用
};